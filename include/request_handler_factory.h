#ifndef REQUEST_HANDLER_FACTORY_H
#define REQUEST_HANDLER_FACTORY_H

#include <string>
#include <map>
#include "request_handler.h"

class RequestHandlerFactory
{
public:
    RequestHandlerFactory(std::map<std::string, std::string> args);
    virtual RequestHandler* create() = 0;
    

protected:
    std::map<std::string, std::string> m_args;

};

#endif