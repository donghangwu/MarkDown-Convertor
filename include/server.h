#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <map>

#include "session.h"
#include "logger.h"
#include "config_parser.h"
#include "request_handler.h"
#include "request_handler_factory.h"
#include "health_request_handler_factory.h"
#include "health_request_handler.h"
#include "sleep_request_handler_factory.h"
#include "sleep_request_handler.h"

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

using boost::asio::ip::tcp;

class server
{
public:
    server(boost::asio::io_service &io_service, NginxConfig &config, bool test = false);
    int start_accept();
    int handle_accept(session *new_session, const boost::system::error_code &error);
    void run();
    ~server();


private:
    std::map<std::string, RequestHandlerFactory *> routes;
    std::map<std::string, std::pair<std::string, std::string>> location_map; // location in config -> actual location of files
    // echo response if value is $echo
    boost::asio::io_service &ioservice;
    tcp::acceptor acceptor;
    bool test;
};

#endif