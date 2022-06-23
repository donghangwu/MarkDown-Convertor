#include "gtest/gtest.h"
#include "markdown_request_handler.h"
#include "markdown/markdown.h"

class MarkdownTestFixture : public ::testing::Test
{
};
TEST_F(MarkdownTestFixture, test_empty_request)
{
    auto markdown_handler = MarkdownRequestHandler();
    http::request<http::string_body> req{http::verb::get, "/markdown", 10};
    req.body() = "## h2 Heading";
    req.prepare_payload();
    std::stringstream req_header_buf;
    req_header_buf << req.base();
    auto req_header = req_header_buf.str();
    auto res = http::response<http::string_body>();
    auto ret = markdown_handler.serve(req, res);
    EXPECT_TRUE(ret == 0);
    EXPECT_EQ("<h2>h2 Heading</h2>\n", res.body());
}

TEST_F(MarkdownTestFixture, test_full_request)
{
    auto markdown_handler = MarkdownRequestHandler();
    http::request<http::string_body> req{http::verb::get, "/markdown", 10};
    req.body() = "And **bold**, *italics*, and even *italics and later **bold***. Even ~~strikethrough~~. [A link](https://markdowntohtml.com) to somewhere.";
    req.prepare_payload();
    std::stringstream req_header_buf;
    req_header_buf << req.base();
    auto req_header = req_header_buf.str();
    auto res = http::response<http::string_body>();
    auto ret = markdown_handler.serve(req, res);
    EXPECT_TRUE(ret == 0);
    EXPECT_EQ("<p>And <strong>bold</strong>, <em>italics</em>, and even <em>italics and later <strong>bold</strong></em>. Even ~~strikethrough~~. <a href=\"https://markdowntohtml.com\">A link</a> to somewhere.</p>\n\n", res.body());
}