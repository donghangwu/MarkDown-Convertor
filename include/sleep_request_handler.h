#ifndef SLEEP_REQUEST_HANDLER_H
#define SLEEP_REQUEST_HANDLER_H

#include "request_handler.h"

class SleepRequestHandler : public RequestHandler
{
public:
    SleepRequestHandler();
    int serve(const http::request<http::string_body> req, http::response<http::string_body> &res);
};

#endif