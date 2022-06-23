#include "gtest/gtest.h"
#include "sleep_request_handler.h"

class SleepTestFixture : public ::testing::Test
{
};
TEST_F(SleepTestFixture, test1)
{
    auto sleep_handler = SleepRequestHandler();
    http::request<http::string_body> req{http::verb::get, "/sleep", 10};
    req.prepare_payload();
    auto res = http::response<http::string_body>();
    auto ret = sleep_handler.serve(req, res);
    // EXPECT_TRUE(ret == 0);
    EXPECT_EQ("Sleeping", res.body());
}
