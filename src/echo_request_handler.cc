#include "echo_request_handler.h"

EchoRequestHandler::EchoRequestHandler()
{
}

int EchoRequestHandler::serve(const http::request<http::string_body> req, http::response<http::string_body> &res)
{

    res = {http::status::ok, req.version()};
    res.set(http::field::content_type, "text/plain");
    std::stringstream header_buf;
    header_buf << req.base();   // convert header to string
    res.body() = header_buf.str() + req.body();
    res.prepare_payload();
    
    return 0;
}