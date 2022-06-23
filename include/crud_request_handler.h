#ifndef CRUD_REQUEST_HANDLER_H
#define CRUD_REQUEST_HANDLER_H

#include <string>
#include <iostream>
#include "request_handler.h"
#include "dirent.h"
#include "fs_wrapper.h"
#include "logger.h"

class CrudRequestHandler : public RequestHandler 
{
public:
    Logger *current_logger = Logger::getLogger();
    CrudRequestHandler(std::string &fs_path, std::string &url_path, IFSWrapper *fp, OFSWrapper *of);
    int serve(const http::request<http::string_body> req,
                    http::response<http::string_body> &res);
    int read_file(std::string file_path, std::string &content);
    int read_dir(std::string file_path, std::string &content);
    void write_file(std::string &file_path, std::string &file_content);

private:
    IFSWrapper *open_file(std::string &file_path);
    // Alternate filestream that can be passed through dependency injection
    IFSWrapper *if_file = nullptr;
    OFSWrapper *of_file = nullptr;
    bool get_filepath(const http::request<http::string_body> &req, std::string &file_path);
    int handleGet(std::string &file_path, http::response<http::string_body> &res);
    int handlePost(std::string &file_path, http::response<http::string_body> &res, std::string &req_body);
    int handlePut(std::string &file_path, http::response<http::string_body> &res, const http::request<http::string_body> &req);
    int handleDelete(std::string &file_path, http::response<http::string_body> &res);
    std::string m_fs_path;
    std::string m_url_path;
};

#endif