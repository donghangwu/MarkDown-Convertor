// // An nginx config file parser.
// //
// // See:
// //   http://wiki.nginx.org/Configuration
// //   http://blog.martinfjordvald.com/2010/07/nginx-primer/
// //
// // How Nginx does it:
// //   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <map>
#include "config_parser.h"

int NginxConfig::GetPortNum()
{
    std::string config_file_str = this->ToString();
    if (config_file_str.find("port") != std::string::npos)
    {
        int start_pos = config_file_str.find("port");
        start_pos += 5;
        int end_pos = config_file_str.find(';', start_pos);
        end_pos += 1;

        int end = end_pos - start_pos - 1;
        int ret = stoi(config_file_str.substr(start_pos, end));
        return ret;
    }
    else if (config_file_str.find("listen") != std::string::npos)
    {
        int start_pos = config_file_str.find("listen");
        start_pos += 7;
        int end_pos = config_file_str.find(';', start_pos);
        end_pos += 1;

        // std::cout << config_file_str.substr(start_pos, end_pos - start_pos - 1);
        // return stoi(config_file_str.substr(start_pos, end_pos - start_pos - 1));
        int end = end_pos - start_pos - 1;
        int ret = stoi(config_file_str.substr(start_pos, end));
        return ret;
    }
    else
    {
        std::cerr << "cannot find port number in config file\n";
        return -1;
    }

    return -1;
}
std::map<std::string, std::pair<std::string, std::string>> NginxConfig::GetLocation()
{

    std::map<std::string, std::pair<std::string, std::string>> request_url_file_path;
    for (const auto &line : statements_)
    {

        if (line->tokens_.size() == 3 &&
            line->tokens_.front() == "location" &&
            line->child_block_.get())
        {
            // location /static StaticHandler
            // take StaticHandler as handlerName
            std::string request_url = line->tokens_.at(1);
            std::string handlerName = line->tokens_.back();
            if (handlerName == "EchoHandler" || handlerName == "HealthHandler" || handlerName == "SleepHandler" || handlerName == "MarkDownHandler")
            {
                request_url_file_path[request_url] = std::make_pair(handlerName, "");
            }
            for (const auto &get_token : line->child_block_->statements_)
            {
                if (get_token->tokens_.empty() != true)
                {
                    std::string target = get_token->tokens_.front();

                    if (target == "root" || target == "data_path")
                    {
                        if (get_token->tokens_.size() == 2)
                        {
                            // take a pair of handlerName and file path
                            request_url_file_path[request_url] = std::make_pair(handlerName, get_token->tokens_.back());
                            break;
                        }
                    }
                }
            }
        }
    }
    return request_url_file_path;
}

std::string NginxConfig::ToString(int depth)
{
    std::string serialized_config;
    for (const auto &statement : statements_)
    {
        serialized_config.append(statement->ToString(depth));
    }
    return serialized_config;
}

std::string NginxConfigStatement::ToString(int depth)
{
    std::string serialized_statement;
    for (int i = 0; i < depth; ++i)
    {
        serialized_statement.append("  ");
    }
    for (unsigned int i = 0; i < tokens_.size(); ++i)
    {
        if (i != 0)
        {
            serialized_statement.append(" ");
        }
        serialized_statement.append(tokens_[i]);
    }
    if (child_block_.get() != nullptr)
    {
        serialized_statement.append(" {\n");
        serialized_statement.append(child_block_->ToString(depth + 1));
        for (int i = 0; i < depth; ++i)
        {
            serialized_statement.append("  ");
        }
        serialized_statement.append("}");
    }
    else
    {
        serialized_statement.append(";");
    }
    serialized_statement.append("\n");
    return serialized_statement;
}

const char *NginxConfigParser::TokenTypeAsString(TokenType type)
{
    switch (type)
    {
    case TOKEN_TYPE_START:
        return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:
        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:
        return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:
        return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:
        return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END:
        return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:
        return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:
        return "TOKEN_TYPE_ERROR";
    default:
        return "Unknown token type";
    }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream *input,
                                                           std::string *value)
{
    TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
    while (input->good())
    {
        const char c = input->get();
        if (!input->good())
        {
            break;
        }
        switch (state)
        {
        case TOKEN_STATE_INITIAL_WHITESPACE:
            switch (c)
            {
            case '{':
                *value = c;
                return TOKEN_TYPE_START_BLOCK;
            case '}':
                *value = c;
                return TOKEN_TYPE_END_BLOCK;
            case '#':
                *value = c;
                state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
                continue;
            case '"':
                *value = c;
                state = TOKEN_STATE_DOUBLE_QUOTE;
                continue;
            case '\'':
                *value = c;
                state = TOKEN_STATE_SINGLE_QUOTE;
                continue;
            case ';':
                *value = c;
                return TOKEN_TYPE_STATEMENT_END;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                continue;
            default:
                *value += c;
                state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
                continue;
            }
        case TOKEN_STATE_SINGLE_QUOTE:

            *value += c;
            if (c == '\'')
            {
                // check for next char
                // the end of a quoted token should be followed by whitespace.
                char next_char = input->peek();
                // if's it's not one of the below characters return error
                if (next_char != ' ' && next_char != ';' && next_char != '{' && next_char != '}' && next_char != '\t' && next_char != '\n')
                {
                    return TOKEN_TYPE_ERROR;
                }
                return TOKEN_TYPE_NORMAL;
            }
            else if (c == '\\') // if escape inside single quote
            {
                state = TOKEN_STATE_SINGLE_QUOTE_WITH_ESCAPE;
            }
            continue;
        case TOKEN_STATE_DOUBLE_QUOTE:
            *value += c;
            if (c == '"')
            {
                // check for next char
                // the end of a quoted token should be followed by whitespace.
                char next_char = input->peek();
                // if's it's not one of the below characters return error
                if (next_char != ' ' && next_char != ';' && next_char != '{' && next_char != '}' && next_char != '\t' && next_char != '\n')
                {
                    return TOKEN_TYPE_ERROR;
                }

                return TOKEN_TYPE_NORMAL;
            }
            else if (c == '\\') // if escape inside single quote
            {
                state = TOKEN_STATE_DOUBLE_QUOTE_WITH_ESCAPE;
            }
            continue;
            // Allow for backslash-escaping within strings.
        case TOKEN_STATE_SINGLE_QUOTE_WITH_ESCAPE:
            *value += c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
        case TOKEN_STATE_DOUBLE_QUOTE_WITH_ESCAPE:
            *value += c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
        case TOKEN_STATE_TOKEN_TYPE_COMMENT:
            if (c == '\n' || c == '\r')
            {
                return TOKEN_TYPE_COMMENT;
            }
            *value += c;
            continue;
        case TOKEN_STATE_TOKEN_TYPE_NORMAL:
            if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
                c == ';' || c == '{' || c == '}')
            {
                input->unget();
                return TOKEN_TYPE_NORMAL;
            }
            *value += c;
            continue;
        }
    }

    // If we get here, we reached the end of the file.
    if (state == TOKEN_STATE_SINGLE_QUOTE ||
        state == TOKEN_STATE_DOUBLE_QUOTE)
    {
        return TOKEN_TYPE_ERROR;
    }

    return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream *config_file, NginxConfig *config)
{
    std::stack<NginxConfig *> config_stack;
    config_stack.push(config);
    TokenType last_token_type = TOKEN_TYPE_START;
    TokenType token_type;
    while (true)
    {
        std::string token;
        token_type = ParseToken(config_file, &token);
        // printf("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
        if (token_type == TOKEN_TYPE_ERROR)
        {
            break;
        }

        if (token_type == TOKEN_TYPE_COMMENT)
        {
            // Skip comments.
            continue;
        }

        if (token_type == TOKEN_TYPE_START)
        {
            // Error.
            break;
        }
        else if (token_type == TOKEN_TYPE_NORMAL)
        {
            if (last_token_type == TOKEN_TYPE_START ||
                last_token_type == TOKEN_TYPE_STATEMENT_END ||
                last_token_type == TOKEN_TYPE_START_BLOCK ||
                last_token_type == TOKEN_TYPE_END_BLOCK ||
                last_token_type == TOKEN_TYPE_NORMAL)
            {
                if (last_token_type != TOKEN_TYPE_NORMAL)
                {
                    config_stack.top()->statements_.emplace_back(
                        new NginxConfigStatement);
                }
                config_stack.top()->statements_.back().get()->tokens_.push_back(
                    token);
            }
            else
            {
                // Error.
                break;
            }
        }
        else if (token_type == TOKEN_TYPE_STATEMENT_END)
        {
            if (last_token_type != TOKEN_TYPE_NORMAL)
            {
                // Error.
                break;
            }
        }
        else if (token_type == TOKEN_TYPE_START_BLOCK)
        {
            if (last_token_type != TOKEN_TYPE_NORMAL)
            {
                // Error.
                break;
            }
            NginxConfig *const new_config = new NginxConfig;
            config_stack.top()->statements_.back().get()->child_block_.reset(
                new_config);
            config_stack.push(new_config);
        }
        else if (token_type == TOKEN_TYPE_END_BLOCK)
        {
            //{}IS valid, }} is valid in some cases. ;} id defintely valid
            if (last_token_type != TOKEN_TYPE_STATEMENT_END && last_token_type != TOKEN_TYPE_END_BLOCK && last_token_type != TOKEN_TYPE_START_BLOCK)
            {
                // Error.
                break;
            }
            config_stack.pop();
        }
        else if (token_type == TOKEN_TYPE_EOF)
        {
            if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
                last_token_type != TOKEN_TYPE_END_BLOCK && last_token_type != TOKEN_TYPE_START) // empty config is true
            {
                // Error.
                break;
            }
            // pop out the TOKEN_TYPE_EOF
            config_stack.pop();
            // if this is the last TOKEN return true,
            if (config_stack.size() == 0)
                return true;
        }
        else
        {
            // Error. Unknown token.
            break;
        }
        last_token_type = token_type;
    }

    printf("Bad transition from %s to %s\n",
           TokenTypeAsString(last_token_type),
           TokenTypeAsString(token_type));
    return false;
}

bool NginxConfigParser::Parse(const char *file_name, NginxConfig *config)
{
    std::ifstream config_file;
    config_file.open(file_name);
    if (!config_file.good())
    {
        printf("Failed to open config file: %s\n", file_name);
        return false;
    }

    const bool return_value =
        Parse(dynamic_cast<std::istream *>(&config_file), config);
    config_file.close();
    return return_value;
}
