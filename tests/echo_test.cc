#include "gtest/gtest.h"
#include "echo_request_handler.h"

class EchoTestFixture : public ::testing::Test
{
};
TEST_F(EchoTestFixture, test_empty_request)
{
    auto echo_handler = EchoRequestHandler();
    http::request<http::string_body> req{http::verb::get, "/echo", 10};
    req.prepare_payload();
    std::stringstream req_header_buf;
    req_header_buf << req.base();
    auto req_header = req_header_buf.str();
    auto res = http::response<http::string_body>();
    auto ret = echo_handler.serve(req, res);
    EXPECT_TRUE(ret == 0);
    EXPECT_EQ(req_header, res.body());
}
TEST_F(EchoTestFixture, test_request_with_body)
{
    auto echo_handler = EchoRequestHandler();
    http::request<http::string_body> req{http::verb::get, "/echo", 10};
    req.body() = "hello";
    req.prepare_payload();
    std::stringstream req_header_buf;
    req_header_buf << req.base();
    auto req_header = req_header_buf.str();
    auto res = http::response<http::string_body>();
    auto ret = echo_handler.serve(req, res);
    EXPECT_TRUE(ret == 0);
    EXPECT_EQ(req_header + req.body(), res.body());
}