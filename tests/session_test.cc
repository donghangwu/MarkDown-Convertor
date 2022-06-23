#include "gtest/gtest.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "request_handler.h"
#include "request_handler_factory.h"
#include "echo_request_handler.h"
#include "not_found_request_handler.h"
#include "static_request_handler.h"
#include "echo_request_handler_factory.h"
#include "static_request_handler_factory.h"
#include "not_found_request_handler_factory.h"
#include "session.h"

class SessionTestFixture : public ::testing::Test
{
protected:
    boost::asio::io_service io_service;
    std::map<std::string, RequestHandlerFactory *> routes;
};

TEST_F(SessionTestFixture, send_res_fail)
{
    routes["/echo"] = new EchoRequestHandlerFactory(std::map<std::string, std::string>());
    session *new_session = new session(io_service, true, routes);
    io_service.run();

    int code = new_session->send_res(boost::system::error_code(), 0);

    EXPECT_TRUE(code == 0);
}

TEST_F(SessionTestFixture, send_static_res_fail)
{
    routes["/static"] = new StaticRequestHandlerFactory(std::map<std::string, std::string>());
    session *new_session = new session(io_service, true, routes);
    io_service.run();

    int code = new_session->send_res(boost::system::error_code(), 0);
    std::cout << "send_static_res_fail code: " << code << std::endl;

    EXPECT_TRUE(code == 0);
}

TEST_F(SessionTestFixture, send_not_found_res_fail)
{
    routes["/non_exist"] = new Not_Found_RequestHandlerFactory(std::map<std::string, std::string>());
    session *new_session = new session(io_service, true, routes);
    io_service.run();

    int code = new_session->send_res(boost::system::error_code(), 0);
    std::cout << "send_not_found_res_fail code: " << code << std::endl;

    EXPECT_TRUE(code == 0);
}

TEST_F(SessionTestFixture, send_res_error)
{

    session *new_session = new session(io_service);
    io_service.run();

    int code = new_session->send_res(make_error_code(boost::system::errc::not_connected), 0);

    EXPECT_TRUE(code == 1);
}
TEST_F(SessionTestFixture, write_callback_error)
{

    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    int ret = new_session->write_callback(make_error_code(boost::system::errc::not_connected));

    EXPECT_TRUE(ret == 1);
}
TEST_F(SessionTestFixture, write_callback_sucess)
{

    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    int ret = new_session->write_callback(boost::system::error_code());

    EXPECT_TRUE(ret == 0);
}

TEST_F(SessionTestFixture, session_match)
{

    std::map<std::string, std::pair<std::string, std::string>> locations;
    std::map<std::string, RequestHandlerFactory *> myroutes;
    locations["/echo"] = std::make_pair("EchoHandler", "");
    locations["/static"] = std::make_pair("StaticHandler", "./static");
    // Set up new session and run io_service
    session *new_session = new session(io_service, true, myroutes, locations);
    io_service.run();
    std::vector<std::string> ret;
    ret = new_session->match("/echo");
    int code = new_session->send_res(boost::system::error_code(), 0);
    EXPECT_TRUE(ret[0] == "/echo");
}

TEST_F(SessionTestFixture, check_access)
{
    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    new_session->check_authorized_access("/api/12345/1");
}

TEST_F(SessionTestFixture, signup)
{
    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    new_session->handle_signup("/sign-up?j_username=yqzhan&j_password=123456");
}

TEST_F(SessionTestFixture, signupfails)
{
    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    new_session->handle_signup("/sign-up?j_username=yqzhan&j_password=123456");
}

TEST_F(SessionTestFixture, signin)
{
    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    new_session->handle_signin("/sign-in?j_username=yqzhan&j_password=123456");
}

TEST_F(SessionTestFixture, signinfails)
{
    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    new_session->handle_signin("/sign-in?j_username=lychee&j_password=123456");
}

TEST_F(SessionTestFixture, update_user)
{
    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    new_session->handle_update_user("lychee");
}

TEST_F(SessionTestFixture, get_user)
{
    // Set up new session and run io_service
    session *new_session = new session(io_service, true);
    io_service.run();
    new_session->handle_get_user("0.0.0.0");
}

TEST_F(SessionTestFixture, auth_success)
{
    // Set up new session and run io_service
    std::map<std::string, std::pair<std::string, std::string>> locations;
    std::map<std::string, RequestHandlerFactory *> myroutes;
    std::map<std::string, std::string> arg_map = std::map<std::string, std::string>();
        arg_map.insert(std::pair<std::string, std::string>("url_path", ""));
        arg_map.insert(std::pair<std::string, std::string>("handler_type", ""));
        arg_map.insert(std::pair<std::string, std::string>("fs_path", ""));
    myroutes["/"] = new StaticRequestHandlerFactory(arg_map);
    locations["/"] = std::make_pair("StaticHandler", "./");

    session *new_session = new session(io_service, true, myroutes, locations);
    http::request<http::string_body> req{http::verb::get, "/static/h.html", 10};
    req.prepare_payload();
    io_service.run();
    new_session->handle_auth_success(req, true);
}

TEST_F(SessionTestFixture, auth_fails)
{
    // Set up new session and run io_service
    std::map<std::string, std::pair<std::string, std::string>> locations;
    std::map<std::string, RequestHandlerFactory *> myroutes;
    std::map<std::string, std::string> arg_map = std::map<std::string, std::string>();
        arg_map.insert(std::pair<std::string, std::string>("url_path", ""));
        arg_map.insert(std::pair<std::string, std::string>("handler_type", ""));
        arg_map.insert(std::pair<std::string, std::string>("fs_path", ""));
    myroutes["/"] = new StaticRequestHandlerFactory(arg_map);
    locations["/"] = std::make_pair("StaticHandler", "./");

    session *new_session = new session(io_service, true, myroutes, locations);
    http::request<http::string_body> req{http::verb::get, "/", 10};
    req.prepare_payload();
    io_service.run();
    new_session->handle_auth_fails(req, true);
}
