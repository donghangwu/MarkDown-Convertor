#include "markdown_request_handler.h"
MarkdownRequestHandler::MarkdownRequestHandler()
{
}

int MarkdownRequestHandler::serve(const http::request<http::string_body> req, http::response<http::string_body> &res)
{

    res = {http::status::ok, req.version()};
    res.set(http::field::content_type, "text/plain");

    std::stringstream markdownInput, htmlout;
    // create markdown object for converting
    markdown::Document doc;
    markdownInput << req.body();

    doc.read(markdownInput);
    doc.write(htmlout);
    res.body() = htmlout.str();
    res.prepare_payload();

    return 0;
}