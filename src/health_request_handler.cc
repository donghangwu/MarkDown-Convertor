#include "health_request_handler.h"

HealthRequestHandler::HealthRequestHandler()
{
}

int HealthRequestHandler::serve(const http::request<http::string_body> req, http::response<http::string_body> &res)
{

    res.version(req.version());
    res.result(http::status::ok);
    res.set(http::field::content_type, "text/plain");
    res.body() = "OK";
    res.prepare_payload();

    return 0;
}
