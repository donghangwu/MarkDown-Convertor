#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <map>
#include "logger.h"
#include "request_handler_factory.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <regex>

using boost::asio::ip::tcp;

class session
{
public:
    typedef std::map<std::string, std::pair<std::string, std::string>> loc_map_type;

    session(boost::asio::io_service &ioservice,
            bool test = false,
            const std::map<std::string, RequestHandlerFactory*> &routes = std::map<std::string, RequestHandlerFactory*>(),

            const loc_map_type &loc_map = loc_map_type());
    int send_res(const boost::system::error_code &error, size_t bytes_transferred);
    int write_callback(const boost::system::error_code &error);
    std::vector<std::string> match(std::string req_path);
    void handle_read();
    void handle_write(http::response<http::string_body> &res);
    void start();
    void check_authorized_access(std::string req_path);
    void handle_signup(std::string req_path);
    void handle_signin(std::string req_path);
    void handle_update_user(std::string client_ip);
    void handle_get_user(std::string client_ip);
    int handle_auth_success(const http::request<http::string_body> req, bool test);
    int handle_auth_fails(const http::request<http::string_body> req, bool test);
    
    tcp::socket socket_;

private:
    loc_map_type location_map;
    std::map<std::string, RequestHandlerFactory*> myroutes;

    std::shared_ptr<http::response<http::string_body>> m_res;
    std::string m_user_name;
    bool test;
    enum
    {
        max_length = 8192
    };
    char data_[max_length];

    bool SIGN;
    bool AUTH_SUCCESS;
    bool BLOCK;
};

#endif