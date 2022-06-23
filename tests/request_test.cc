#include "gtest/gtest.h"
#include "request.h"

class RequestTestFixture : public ::testing::Test
{
};
TEST_F(RequestTestFixture, test_empty_request)
{
    std::string request_message = "";
    Request r(request_message);
    EXPECT_TRUE(r.get_Method() != 0 && r.get_Path() != 0 && r.get_Body() != 0);
}
TEST_F(RequestTestFixture, test_incomplete_request)
{
    std::string request_message = "GET /static HTTP/1.1";
    Request r(request_message);
    EXPECT_TRUE(r.get_Method() == 0 && r.get_Path() == 0 && r.get_Body() != 0);
}
TEST_F(RequestTestFixture, test_incomplete_request_2)
{
    std::string request_message = "GET /static HTTP/1.1\r\nHost: 35.230.27.6";
    Request r(request_message);
    EXPECT_TRUE(r.get_Method() == 0 && r.get_Path() == 0 && r.get_Body() != 0);
}
TEST_F(RequestTestFixture, test_complete_request)
{
    std::string request_message = "GET /static HTTP/1.1\r\nHost: 35.230.27.6\r\n\r\n";
    Request r(request_message);
    EXPECT_TRUE(r.ParseRequest() == 0);
}
TEST_F(RequestTestFixture, test_invalid_path)
{
    std::string request_message = "GET /wrong HTTP/1.1\r\nHost: 35.230.27.6\r\n\r\n";
    Request r(request_message);
    EXPECT_TRUE(r.get_Method() == 0 && r.get_Path() == 0 && r.get_Body() == 0 && r.ParseRequest() == 0);
}
TEST_F(RequestTestFixture, test_echo)
{
    std::string request_message = "GET /echo HTTP/1.1\r\nHost: 35.230.27.6\r\n\r\n";
    Request r(request_message);
    EXPECT_TRUE(r.get_Method() == 0 && r.get_Path() == 0 && r.get_Body() == 0 && r.ParseRequest() == 0);
}