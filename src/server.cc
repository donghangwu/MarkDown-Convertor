#include "server.h"
#include "static_request_handler_factory.h"
#include "echo_request_handler_factory.h"
#include "crud_request_handler_factory.h"
#include "not_found_request_handler_factory.h"
#include "health_request_handler_factory.h"
#include "sleep_request_handler_factory.h"
#include "markdown_request_handler_factory.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

// overloaded constructor that doesnt start_accept
server::server(boost::asio::io_service &io_service, NginxConfig &config, bool test)
    : ioservice(io_service),
      acceptor(io_service, tcp::endpoint(tcp::v4(), config.GetPortNum())),
      test(test)
{
    location_map = config.GetLocation();
    // location_map =<string, pair(string,string)>
    //               /static  pair("staticHandler",./static)
    //             recived_request_path  (requestName, local file path)

    for (std::map<std::string, std::pair<std::string, std::string>>::reverse_iterator iter = location_map.rbegin(); iter != location_map.rend(); iter++)
    {
        std::string request_path = iter->first;       // after "location" = /static
        std::string handlerName = iter->second.first; // StaticHandler
        std::string config_file_path = iter->second.second;

        std::map<std::string, std::string> arg_map = std::map<std::string, std::string>();
        arg_map.insert(std::pair<std::string, std::string>("url_path", request_path));
        arg_map.insert(std::pair<std::string, std::string>("handler_type", handlerName));
        arg_map.insert(std::pair<std::string, std::string>("fs_path", config_file_path));

        if (handlerName == "StaticHandler")
        {
            routes[request_path] = new StaticRequestHandlerFactory(arg_map);
        }
        else if (handlerName == "EchoHandler")
        {
            routes[request_path] = new EchoRequestHandlerFactory(arg_map);
        }
        else if (handlerName == "CrudHandler")
        {
            routes[request_path] = new CrudRequestHandlerFactory(arg_map);
        }
        else if (handlerName == "HealthHandler")
        {
            routes[request_path] = new HealthRequestHandlerFactory(arg_map);
        }
        else if (handlerName == "SleepHandler")
        {
            routes[request_path] = new SleepRequestHandlerFactory(arg_map);
        }
        else if (handlerName == "MarkDownHandler")
        {
            routes[request_path] = new Markdown_RequestHandlerFactory(arg_map);
        }

        routes["NotFoundHandler"] = new Not_Found_RequestHandlerFactory(arg_map);
    }
}

server::~server()
{
    ioservice.stop();
}

void server::run()
{
    // Create a pool of threads to run all of the io_services.
    std::vector<boost::shared_ptr<boost::thread>> threads;
    for (std::size_t i = 0; i < boost::thread::hardware_concurrency(); ++i)
    {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, &ioservice)));
        threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i)
        threads[i]->join();
}

int server::start_accept()
{
    session *new_session = new session(ioservice, false, routes, location_map);
    acceptor.async_accept(new_session->socket_,
                          boost::bind(&server::handle_accept, this, new_session,
                                      boost::asio::placeholders::error));
    return 0;
}

int server::handle_accept(session *new_session, const boost::system::error_code &error)
{
    if (!error)
    {
        new_session->start();
    }
    else
    {
        delete new_session;
        return -1;
    }

    if (!test)
        start_accept(); // wait for the next session

    return 0;
}