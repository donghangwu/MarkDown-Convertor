#ifndef MARKDOWN_REQUEST_HANDLER_H
#define MARKDOWN_REQUEST_HANDLER_H

#include "request_handler.h"
#include "markdown/markdown.h"
#include <string>
#include <sstream>
#include <fstream>

class MarkdownRequestHandler : public RequestHandler
{
public:
    MarkdownRequestHandler();
    int serve(const http::request<http::string_body> req, http::response<http::string_body> &res);
};

#endif