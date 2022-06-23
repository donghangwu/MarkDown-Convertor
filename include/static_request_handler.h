#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include <fstream>

#include "request_handler.h"
#include "not_found_request_handler.h"
class StaticRequestHandler : public RequestHandler
{
public:
    StaticRequestHandler(std::string fs_path, std::string url_path);
    int serve(const http::request<http::string_body> req, http::response<http::string_body> &res);

    std::string get_mime_type(std::string file_ext);
    int read_file(std::string file_path, std::string &file_content);

private:
    std::string m_fs_path;
    std::string m_url_path;
};

#endif