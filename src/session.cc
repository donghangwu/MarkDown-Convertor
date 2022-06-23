#include "session.h"

#define SIGNUP "/sign-up?j_username="
#define SIGNIN "/sign-in?j_username="
#define AUTH_PATH "./crud/admin/ip_usr"

session::session(boost::asio::io_service &ioservice, bool test, const std::map<std::string, RequestHandlerFactory *> &routes, const loc_map_type &loc_map)
    : socket_(ioservice), test(test), location_map(loc_map), myroutes(routes)
{
    m_user_name = "";
    SIGN = false;
    AUTH_SUCCESS = false; 
    BLOCK = false;
}

void session::start()
{

    Logger *current_logger = Logger::getLogger();
    current_logger->logTraceMSG("Session started.");
    handle_read();
}
// read in request
void session::handle_read()
{
    Logger *current_logger = Logger::getLogger();
    current_logger->logTraceMSG("Handling read.");
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&session::send_res, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

std::vector<std::string> resplit(const std::string &s, const std::regex &sep_regex = std::regex{"\\s+"}) {
  std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
  std::sregex_token_iterator end;
  return {iter, end};
}

/* callback function to send result back to client */
int session::send_res(const boost::system::error_code &error, size_t bytes_transferred)
{
    Logger *current_logger = Logger::getLogger();
    SIGN = false;
    AUTH_SUCCESS = false; 
    BLOCK = false;

    current_logger->logTraceMSG("Sending response back to the client.");
    if (!error)
    {
        http::request<http::string_body> req;
        http::parser<true, http::string_body> req_parser;
        beast::error_code ec;

        // parse the data
        auto bytes_parsed = req_parser.put(boost::asio::buffer(data_, bytes_transferred), ec);
        // if request data is invalid, return 400 Bad Request
        if (ec)
        {
            current_logger->logErrorMSG("Error occured while parsing the request.");
            current_logger->logErrorMSG("Error: " + ec.message());

            auto mal_res = std::make_shared<http::response<http::string_body>>();

            std::string body = "400 Bad Request\n";

            mal_res->result(http::status::bad_request);
            mal_res->set(http::field::content_type, "text/plain");
            mal_res->set(http::field::content_length, body.length());
            mal_res->body() = body;

            m_res = mal_res; // assign to memeber variable to avoid out of scope issue after async write
                             // machine-parsable log output

            std::string client_ip_address = "127.0.0.1";
            if (!test)
                client_ip_address = socket_.remote_endpoint().address().to_string();

            std::stringstream ss;
            ss << "[ResponseMetrics]: "
               << "\tResponse code: " << std::to_string(mal_res->result_int()) << " " << (mal_res->result()) << "\tRequest path: " << req.target() << "\tRequest IP: " << client_ip_address << "\tMatched request handler name: BadRequestsHandler";
            current_logger->logInfoMSG(ss.str());

            handle_write(*mal_res);

            return 0;
        }
        else
        {
            // create http request object from parser
            req = req_parser.get();
            if (bytes_parsed < bytes_transferred)
            {
                // For some reason, the parser refuse to parse the body but only parse the headers
                // manually assign the body
                req.body() = data_ + bytes_parsed;
            }

            try
            {

                std::string client_ip_address = socket_.remote_endpoint().address().to_string();
                current_logger->logRequest(req, client_ip_address);
                // match the current ip address to the user name
                // if (m_user_name == "")
                // {
                handle_get_user(client_ip_address);
                // }
            }
            catch (std::exception &e)
            {
                std::cerr << "Exception: logRequest Error: " << e.what() << "\n";
            }
            // Check if it is authorized access
            check_authorized_access(std::string(req.target()));

            // dispatching!---------------------------------------------------------!
            
            if (std::string(req.target()).length() >= 20)
            {
                // if this is from a signup form
                if (std::string(req.target()).find(SIGNUP) != std::string::npos) handle_signup(std::string(req.target()));
                // if this is from a signin form
                else if (std::string(req.target()).find(SIGNIN) != std::string::npos) handle_signin(std::string(req.target()));
            }
            
            auto status = 0;
            std::vector<std::string> locations;
            if (AUTH_SUCCESS && SIGN)
            {
                locations = match("/");
                status = handle_auth_success(req, false);
            }
            else if (!AUTH_SUCCESS & SIGN)
            {
                locations = match("/");
                status = handle_auth_fails(req, false);
            }
            else if (!BLOCK)
            {
                locations = match(std::string(req.target()));
                auto *factory = myroutes[locations[0]];
                auto *request_handler = factory->create();
                auto res = std::make_shared<http::response<http::string_body>>();
                status = request_handler->serve(req, *res);
                m_res = res; // assign to memeber variable to avoid out of scope issue after async write
            }
            else
            {
                locations = match(std::string(req.target()));
                status = 1;
            }

            if (status != 0)
            {
                auto *factory  = myroutes["NotFoundHandler"];
                auto *request_handler = factory->create();
                auto res = std::make_shared<http::response<http::string_body>>();
                status = request_handler->serve(req, *res);
                m_res = res;
            }
            // machine-parsable log output
            std::string client_ip_address = socket_.remote_endpoint().address().to_string();
            // if authentication was successful we need to update the current user
            if (AUTH_SUCCESS) handle_update_user(client_ip_address);
            std::stringstream ss;
            ss << "[ResponseMetrics]:\t"
               << "Response code: " << std::to_string(m_res->result_int()) << " " << (m_res->result()) << "\tRequest path: " << req.target() << "\tRequest IP: " << client_ip_address << "\tMatched request handler name : " << locations[1];
            current_logger->logInfoMSG(ss.str());
            handle_write(*m_res);
            memset(data_, 0, sizeof(data_));
        }

        return 0;
    }
    else
    {
        current_logger->logDebugMSG("Error when sending response.");
        delete this;
        return 1;
    }
}
std::vector<std::string> session::match(std::string req_path)
{
    Logger *current_logger = Logger::getLogger();
    std::vector<std::string> locations = {"NotFoundHandler", "", ""};
    for (std::map<std::string, std::pair<std::string, std::string>>::reverse_iterator iter = location_map.rbegin(); iter != location_map.rend(); iter++)
    {
        std::string request_path = iter->first;             // after "location" = /static
        std::string handlerName = iter->second.first;       // StaticHandler
        std::string config_file_path = iter->second.second; // after "root" =./static
        current_logger->logInfoMSG("handlerName: " + handlerName + " reques_path: " + request_path + " filepath: " + config_file_path);
        if (req_path.find(request_path) == 0)
        {
            //  /echos should return 404
            if (handlerName == "EchoHandler" && req_path.substr(req_path.find(request_path)) != request_path)
                continue;
            locations[0] = request_path;
            locations[1] = handlerName;
            locations[2] = config_file_path;
            return locations;
        }
    }
    return locations;
}

void session::check_authorized_access(std::string req_path)
{
    if (req_path.length() >= 4)
    {
        if (req_path.substr(0,4) == "/api")
        {
            std::regex rgx_slash("\/");
            std::vector<std::string> tokens = resplit(req_path, rgx_slash);
            if (tokens.size() >= 3)
            {
                if (tokens[2] != m_user_name || m_user_name == "")
                {
                    BLOCK = true;
                }
            }
        }
    }
}

void session::handle_signup(std::string req_path)
{
    Logger *current_logger = Logger::getLogger();
    current_logger->logDebugMSG("handling signup");
    std::regex rgx("&j_password=");
    std::string substring_req = req_path.substr(20);

    SIGN = true;
    std::vector<std::string> logons = resplit(substring_req, rgx);
    std::string password = logons[1];
    std::string file_path = "./crud/" + logons[0] + "-pwd";
    current_logger->logInfoMSG("User Tryig Sign up: " + m_user_name);
    // create a password file for the entity if this does not exists
    if (boost::filesystem::exists(file_path))
    {
        current_logger->logDebugMSG("auth success");
        AUTH_SUCCESS = false;
    }
    else
    {
        m_user_name = logons[0];
        AUTH_SUCCESS = true;
        boost::filesystem::path entity_path(file_path);
        boost::filesystem::create_directory(entity_path);
        file_path.append("/password");
        std::ofstream f(file_path.c_str(), std::ofstream::trunc); 
        f << password;
        f.close();
        // Create directory for the user
        boost::filesystem::create_directory("./crud/" + logons[0]);
    }
}

void session::handle_signin(std::string req_path)
{
    Logger *current_logger = Logger::getLogger();
    std::regex rgx("&j_password=");
    current_logger->logDebugMSG("handling signin");
    std::string substring_req = req_path.substr(20);
    SIGN = true;
    std::vector<std::string> logons = resplit(substring_req, rgx);
    std::string password = logons[1];
    std::string file_path = "./crud/" + logons[0] + "-pwd/password";
    // check if the password matches
    if (boost::filesystem::exists(file_path))
    {
        current_logger->logDebugMSG("user found");
        std::ifstream ss(file_path);
        std::string true_password = "";
        getline (ss,true_password);
        ss.close();
        if (true_password == password)
        {
            m_user_name = logons[0];
            AUTH_SUCCESS = true;
        }
        else
        {
            AUTH_SUCCESS = false;
        }
    }
    else
    {
        AUTH_SUCCESS = false;
    }
}

void session::handle_update_user(std::string client_ip_address)
{
    Logger *current_logger = Logger::getLogger();
    current_logger->logDebugMSG("appending to admin password file");
    std::ofstream fileOUT(AUTH_PATH, std::ios_base::app); 
    if(!fileOUT)
    {
        current_logger->logErrorMSG("Cannot open the File");
        return ;
    }
    fileOUT << client_ip_address << std::endl; 
    fileOUT << m_user_name << std::endl; 
    fileOUT.close(); // close the file
}

void session::handle_get_user(std::string client_ip_address)
{
    Logger *current_logger = Logger::getLogger();
    std::ifstream in(AUTH_PATH);
    current_logger->logDebugMSG("getting user");
    if(!in)
    {
        current_logger->logErrorMSG("Cannot open the File");
        return ;
    }
    std::string line;
    while (std::getline(in, line))
    {
        if (line == client_ip_address)
        {
            std::getline(in, line);
            m_user_name = line;
        }
    }
    in.close();
}

int session::handle_auth_success(const http::request<http::string_body> req, bool test)
{
    int status;
    Logger *current_logger = Logger::getLogger();
    current_logger->logInfoMSG("auth success");
    auto *factory = myroutes["/"];
    auto *request_handler = factory->create();
    auto res = std::make_shared<http::response<http::string_body>>();
    http::response<http::string_body> &point_res = *res;
    point_res.body() = "/auth_success.html";
    if (test)
    {
        current_logger->logInfoMSG("auth success");
        return 0;
    }
        

    status = request_handler->serve(req, *res);
    m_res = res; // assign to memeber variable to avoid out of scope issue after async write
    return status;
}

int session::handle_auth_fails(const http::request<http::string_body> req, bool test)
{
    int status;
    Logger *current_logger = Logger::getLogger();
    current_logger->logInfoMSG("auth fails");
    auto *factory = myroutes["/"];
    auto *request_handler = factory->create();
    auto res = std::make_shared<http::response<http::string_body>>();
    http::response<http::string_body> &point_res = *res;
    point_res.body() = "/auth_fails.html";
    if (test)
    {
        current_logger->logInfoMSG("auth fails");
        return 0;
    }

    status = request_handler->serve(req, *res);
    m_res = res; // assign to memeber variable to avoid out of scope issue after async write
    return status;
}
// write response back to socket
void session::handle_write(http::response<http::string_body> &res)
{
    Logger *current_logger = Logger::getLogger();
    current_logger->logDebugMSG("Handling write.");
    http::async_write(socket_, res, boost::bind(&session::write_callback, this, boost::asio::placeholders::error));
}

// write call back wait for read next message
int session::write_callback(const boost::system::error_code &error)
{
    Logger *current_logger = Logger::getLogger();
    current_logger->logDebugMSG("Writing callback.");
    if (!error)
    {
        if (!test)
            handle_read();
        return 0;
    }
    else
    {
        current_logger->logDebugMSG("Error when writing callback.");
        delete this;
        return 1;
    }
}
