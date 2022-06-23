#ifndef HEALTH_REQUEST_HANDLER_H
#define HEALTH_REQUEST_HANDLER_H

#include "request_handler.h"

class HealthRequestHandler : public RequestHandler
{
public:
    HealthRequestHandler();
    int serve(const http::request<http::string_body> req, http::response<http::string_body> &res);
};

#endif