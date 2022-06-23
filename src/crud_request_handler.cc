#include "crud_request_handler.h"
#include "request_handler_factory.h"
#include "not_found_request_handler_factory.h"

CrudRequestHandler::CrudRequestHandler(std::string &fs_path, std::string &url_path, IFSWrapper *fp, OFSWrapper *of)
{
    // If IFSWrapper passed through dependency injection, use it
    if (fp != nullptr)
    {
        if_file = fp;
    }
    if (of != nullptr)
    {
        of_file = of;
    }
    m_fs_path = fs_path;
    m_url_path = url_path;
}

int CrudRequestHandler::serve(const http::request<http::string_body> req,
                              http::response<http::string_body> &res)
{
    std::string req_type = req.method_string().to_string();
    std::string req_target = req.target().to_string();
    std::string req_body = req.body();
    std::string file_path;
    if (get_filepath(req, file_path))
    {
        current_logger->logErrorMSG("Error: get_filepath failed");
        return 1;
    }
    res = {http::status::ok, req.version()};
    int status_code = 0;
    try
    {
        if (req_type == "GET")
        {
            status_code = handleGet(file_path, res);
        }
        else if (req_type == "POST")
        {
            status_code = handlePost(file_path, res, req_body);
        }
        else if (req_type == "PUT")
        {
            status_code = handlePut(file_path, res, req);
        }
        else if (req_type == "DELETE")
        {
            status_code = handleDelete(file_path, res);
        }
        return status_code;
    }
    catch (std::exception &e)
    {
        current_logger->logErrorMSG(e.what());
        return 1;
    }
}

// gets filepath from request
bool CrudRequestHandler::get_filepath(const http::request<http::string_body> &req, std::string &file_path)
{
    std::stringstream user_url_buf;
    user_url_buf << req.target();
    std::string user_url_path = user_url_buf.str();

    auto found = user_url_path.find(m_url_path);
    if (found == std::string::npos)
    {
        // Did not find url_path specified in config in the url the user requested (although this should be impossible)
        current_logger->logErrorMSG("user_url_path.find(m_url_path) failed");
        return 1;
    }
    user_url_path.erase(found, m_url_path.length()); // erase url_path, only keep the filename
    file_path = m_fs_path + user_url_path;
    return 0;
}

IFSWrapper *CrudRequestHandler::open_file(std::string &file_path)
{
    // automatically return dependency injection IFSWrapper if it exists
    if (if_file != nullptr)
    {
        return if_file;
    }
    IFSWrapper *file_stream = new IFStream(file_path);
    if (!file_stream->is_open())
    {
        current_logger->logErrorMSG("file_stream->is_open() failed for path: " + file_path);
        return nullptr;
    }
    return file_stream;
}

// read in the file
int CrudRequestHandler::read_file(std::string file_path, std::string &content)
{
    IFSWrapper *file_stream = open_file(file_path);
    if (file_stream == nullptr)
    {
        return 1;
    }
    // read the file
    char ch;
    while (file_stream->get(ch))
        content += ch;
    return 0;
}

// read in the directory
int CrudRequestHandler::read_dir(std::string file_path, std::string &content)
{
    try
    {
        if (auto dir = opendir(file_path.c_str()))
        {
            content = "[";
            while (auto f = readdir(dir))
            {
                if (!f->d_name || f->d_name[0] == '.')
                    continue; // Skip everything that starts with a dot
                content += f->d_name;
                content += ",";
            }
            closedir(dir);
            content.pop_back(); // remove the last comma
            content += "]";
        }
        else
        {
            return 1;
        }
    }
    catch (...)
    {
        current_logger->logErrorMSG("error reading dir: " + file_path);
        return 1;
    }
    return 0;
}

// write the file
void CrudRequestHandler::write_file(std::string &file_path, std::string &req_body)
{
    OFSWrapper *file_stream;
    if (of_file != nullptr)
    {
        file_stream = of_file;
        // create or write into the file
        if (file_stream->is_open())
        {
            (*file_stream).write(req_body);
        }
        file_stream->close();
    }
    else
    {
        file_stream = new OFStream(file_path);
        // create or write into the file
        std::ofstream f(file_path.c_str(), std::ofstream::trunc);
        f << req_body;
        f.close();
    }

}

int CrudRequestHandler::handleGet(std::string &file_path, http::response<http::string_body> &res)
{
    current_logger->logInfoMSG("Received GET request");
    auto content = std::string();
    current_logger->logInfoMSG("Reading entire list of files from: " + file_path);
    auto ret_dir = read_dir(file_path, content);
    // if not a directory
    if (ret_dir)
    {
        // try reading the file
        auto ret = read_file(file_path, content);
        if (ret)
        {
            current_logger->logErrorMSG("get request failed, invalid file and directory");
            return 1;
        }
    }

    res.body() = content;
    res.prepare_payload();
    return 0;
}

int CrudRequestHandler::handlePost(std::string &file_path, http::response<http::string_body> &res, std::string &req_body)
{
    current_logger->logInfoMSG("Received POST request");
    int id = 1;
    // check if directory don't exist, then create file in dir
    if (!boost::filesystem::exists(file_path))
    {
        boost::filesystem::path entity_path(file_path);
        boost::filesystem::create_directory(entity_path);
        file_path.append("/1");
    }
    else
    {
        // else if directory exist, find next id number
        while (1)
        {
            std::string new_path = file_path + "/" + std::to_string(id);
            if (boost::filesystem::exists(new_path))
            {
                id++;
            }
            else
            {
                file_path = new_path;
                break;
            }
        }
    }
    write_file(file_path, req_body);
    res.result(boost::beast::http::status::ok);
    res.set(boost::beast::http::field::content_type, "text/plain");
    res.body() = "{\"id\": " + std::to_string(id) + "}";
    res.prepare_payload();
    return 0;
}

int CrudRequestHandler::handlePut(std::string &file_path, http::response<http::string_body> &res, const http::request<http::string_body> &req)
{
    current_logger->logInfoMSG("Received PUT request");
    if (!boost::filesystem::exists(file_path))
    {
        std::map<std::string, std::string> error_map;
        RequestHandlerFactory *not_found_error_factory = new Not_Found_RequestHandlerFactory(error_map);
        auto error_handler = not_found_error_factory->create();
        error_handler->serve(req, res);
        return 1;
    }
    std::string req_body = req.body();
    write_file(file_path, req_body);
    res.result(boost::beast::http::status::ok);
    res.set(boost::beast::http::field::content_type, "text/plain");
    res.body() = req_body;
    res.prepare_payload();

    return 0;
}

int CrudRequestHandler::handleDelete(std::string &file_path, http::response<http::string_body> &res)
{
    current_logger->logInfoMSG("Received DELETE request");
    res.body() = "Deleted file";
    res.prepare_payload();
    return std::remove(file_path.c_str());
}