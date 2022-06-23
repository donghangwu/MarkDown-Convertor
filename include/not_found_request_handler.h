#ifndef NOT_FOUND_REQUEST_HANDLER_H
#define NOT_FOUND_REQUEST_HANDLER_H

#include "request_handler.h"

class NotFoundRequestHandler : public RequestHandler
{
public:
    NotFoundRequestHandler();
    int serve(const http::request<http::string_body> req, http::response<http::string_body> &res);
};

#endif