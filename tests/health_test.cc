#include "gtest/gtest.h"
#include "health_request_handler.h"

class HealthTestFixture : public ::testing::Test
{
};
TEST_F(HealthTestFixture, test1)
{
    auto health_handler = HealthRequestHandler();
    http::request<http::string_body> req{http::verb::get, "/health", 10};
    req.prepare_payload();
    auto res = http::response<http::string_body>();
    auto ret = health_handler.serve(req, res);
    // EXPECT_TRUE(ret == 0);
    EXPECT_EQ("OK", res.body());
}
