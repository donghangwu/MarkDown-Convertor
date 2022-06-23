#include "gtest/gtest.h"
#include "server.h"
#include "session.h"
#include "config_parser.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>

class ServerTestFixture : public ::testing::Test
{
protected:
    boost::asio::io_service io_service;
    NginxConfig config;
};
TEST_F(ServerTestFixture, test_start_accept)
{
    NginxConfigParser config_parser;
    config_parser.Parse("config_files/ex_config", &config);
    server my_server(io_service, config);
    EXPECT_TRUE(my_server.start_accept() == 0);
}
TEST_F(ServerTestFixture, test_handle_accept_error)
{
    NginxConfigParser config_parser;
    config_parser.Parse("config_files/ex_config", &config);
    server my_server(io_service, config);
    session *new_session = new session(io_service);
    EXPECT_TRUE(my_server.handle_accept(new_session, make_error_code(boost::system::errc::not_connected)) == -1);
}

TEST_F(ServerTestFixture, test_handle_accept_testing)
{
    NginxConfigParser config_parser;
    config_parser.Parse("config_files/ex_config", &config);
    server my_server(io_service, config, true);
    session *new_session = new session(io_service);
    EXPECT_TRUE(my_server.handle_accept(new_session, boost::system::error_code()) == 0);
}
TEST_F(ServerTestFixture, test_handle_accept)
{
    NginxConfigParser config_parser;
    config_parser.Parse("../config_files/ex_config", &config);
    server my_server(io_service, config);
    session *new_session = new session(io_service);
    EXPECT_TRUE(my_server.handle_accept(new_session, boost::system::error_code()) == 0);
}
