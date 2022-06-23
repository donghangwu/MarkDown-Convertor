#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include "request_handler.h"

class EchoRequestHandler : public RequestHandler
{
public:
    EchoRequestHandler();
    int serve(const http::request<http::string_body> req, http::response<http::string_body> &res);

};

#endif