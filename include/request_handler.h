#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <boost/beast/http.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>

class RequestHandler
{
public:
    virtual int serve(const http::request<http::string_body> req, http::response<http::string_body> &res) = 0;

};

#endif