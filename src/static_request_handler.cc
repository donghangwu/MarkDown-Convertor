#include "static_request_handler.h"
#include <iostream>

#define HOME_PAGE "/login.html"
StaticRequestHandler::StaticRequestHandler(std::string fs_path, std::string url_path)
{
    m_fs_path = fs_path;
    m_url_path = url_path;
}

// return the response message based on what's being request in the url
int StaticRequestHandler::serve(const http::request<http::string_body> req, http::response<http::string_body> &res)
{

    std::stringstream user_url_buf;
    if (res.body() == "")
    {
        user_url_buf << req.target();
    }
    else
    {
        user_url_buf << res.body();
    }
    if (req.target() == "/")
    {
        user_url_buf << HOME_PAGE;
    }
    std::string user_url_path = user_url_buf.str();

    auto found = user_url_path.find(m_url_path);
    if (found == std::string::npos)
    {
        // Did not find url_path specified in config in the url the user requested (although this should be impossible)
        return 1;
    }
    user_url_path.erase(found, m_url_path.length()); // erase url_path, only keep the filename
    // if the url is like /index.html/#/1/edit
    // read in as /index.html/, we have to remove the last / to serve the file.
    if (user_url_path.back() == '/')
    {
        user_url_path.pop_back();
    }
    // if url is /index.html, read in as index.html, need to add / infront of file name.
    if (user_url_path[0] != '/')
    {
        user_url_path = "/" + user_url_path;
    }

    auto file_path = m_fs_path + user_url_path;

    // get the file extension
    auto extension = std::string();
    // find the extension
    auto ext_pos = file_path.find_last_of(".");
    if (ext_pos != std::string::npos)
        extension = file_path.substr(ext_pos + 1);
    else
        return 1; // Didn't find file in extension in file name

    auto file_content = std::string();
    auto ret = read_file(file_path, file_content);

    // if we cannot read the file
    if (ret)
    {
        // Evoke not found handler here.
        return 1;
        // NotFoundHandler not_found_msg;
        // return not_found_msg.serve(locations, data);
    }

    // find the content tyoe (jpg,text)
    auto mime_type = get_mime_type(extension);

    res = {http::status::ok, req.version()};
    res.set(http::field::content_type, mime_type);
    res.body() = file_content;
    res.prepare_payload();

    return 0;
}

// return the extension
std::string StaticRequestHandler::get_mime_type(std::string file_ext)
{
    std::string file_content_type;

    if (file_ext == "html" || file_ext == "htm")
        file_content_type = "text/html";
    else if (file_ext == "zip")
        file_content_type = "application/zip";
    else if (file_ext == "jpg" || file_ext == "jpeg")
        file_content_type = "image/jpeg";
    else if (file_ext == "png")
        file_content_type = "image/png";
    else if (file_ext == "js")
        file_content_type = "text/javascript";
    else if (file_ext == "css")
        file_content_type = "text/css";
    else
        file_content_type = "text/plain";

    return file_content_type;
}

// read in the file
int StaticRequestHandler::read_file(std::string file_path, std::string &file_content)
{

    std::ifstream file(file_path, std::ios::in | std::ios::binary);

    // check the path
    if (!file.good())
        return -1;

    // check if the file exists
    if (!file.is_open())
        return -1;

    // read the file
    char ch;
    while (file.get(ch))
        file_content += ch;

    return 0;
}