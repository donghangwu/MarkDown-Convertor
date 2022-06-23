#include "gtest/gtest.h"
#include "not_found_request_handler.h"

class NotFoundTestFixture : public ::testing::Test
{
};
TEST_F(NotFoundTestFixture, test_wrong_url)
{
    auto not_found_handler = NotFoundRequestHandler();
    http::request<http::string_body> req{http::verb::get, "/non_exist_path", 10};
    req.prepare_payload();
    auto res = http::response<http::string_body>();
    auto ret = not_found_handler.serve(req, res);
    // EXPECT_TRUE(ret == 0);
    EXPECT_EQ("cannot reach this site!", res.body());
}
