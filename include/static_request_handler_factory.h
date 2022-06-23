#ifndef STATIC_REQUEST_HANDLER_FACTORY_H
#define STATIC_REQUEST_HANDLER_FACTORY_H

#include <string>
#include <map>
#include "request_handler.h"
#include "echo_request_handler_factory.h"

class StaticRequestHandlerFactory : public RequestHandlerFactory
{
public:
    StaticRequestHandlerFactory(std::map<std::string, std::string> args);
    RequestHandler* create();

};

#endif