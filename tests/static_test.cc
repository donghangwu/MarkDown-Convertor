#include "gtest/gtest.h"
#include "static_request_handler.h"
// #include <boost/asio.hpp>
// #include <iostream>
// #include <string>
// #include <filesystem>
// #include <unistd.h>
// #include <vector>
// using boost::asio::ip::tcp;

class StaticTestFixture : public ::testing::Test
{

protected:
    std::string m_fs_path = "./static";
    std::string m_url_path = "/static";
};

TEST_F(StaticTestFixture, test_HTML)
{

    auto handler = StaticRequestHandler(m_fs_path, m_url_path);
    http::request<http::string_body> req{http::verb::get, "/static/h.html", 10};
    req.prepare_payload();
    auto res = http::response<http::string_body>();
    auto ret = handler.serve(req, res);
    std::ifstream f(m_fs_path + "/h.html");
    std::stringstream buffer;
    buffer << f.rdbuf();
    EXPECT_TRUE(ret == 0);
    EXPECT_EQ(buffer.str(), res.body());
}
TEST_F(StaticTestFixture, test_txt)
{
    auto handler = StaticRequestHandler(m_fs_path, m_url_path);
    http::request<http::string_body> req{http::verb::get, "/static/text.txt", 10};
    req.prepare_payload();
    auto res = http::response<http::string_body>();
    auto ret = handler.serve(req, res);
    std::ifstream f(m_fs_path + "/text.txt");
    std::stringstream buffer;
    buffer << f.rdbuf();
    EXPECT_TRUE(ret == 0);
    EXPECT_EQ(buffer.str(), res.body());
}

// TEST_F(StaticTestFixture, test_notExist)
// {
//     std::string data = "GET /static/not_exist.txt HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\nCache-Control: max-age=0";

//     StaticRequestHandler static_handler;

//     std::string response = static_handler.serve(locations, data);

//     std::string expected_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 24\r\n\r\nCannot reach this site\r\n";
//     EXPECT_TRUE(response == expected_response);
// }
