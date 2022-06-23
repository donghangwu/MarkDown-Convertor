#include "not_found_request_handler.h"

NotFoundRequestHandler::NotFoundRequestHandler()
{
}

int NotFoundRequestHandler::serve(const http::request<http::string_body> req, http::response<http::string_body> &res)
{

    res = {http::status::not_found, req.version()};
    res.set(http::field::content_type, "text/plain");
    std::stringstream header_buf;
    header_buf << req.base();               // convert header to string
    res.body() = "cannot reach this site!"; // header_buf.str() + req.body();
    res.prepare_payload();

    return 0;
}