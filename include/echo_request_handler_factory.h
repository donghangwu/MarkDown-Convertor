#ifndef ECHO_REQUEST_HANDLER_FACTORY_H
#define ECHO_REQUEST_HANDLER_FACTORY_H

#include <string>
#include <map>
#include "request_handler.h"
#include "request_handler_factory.h"

class EchoRequestHandlerFactory : public RequestHandlerFactory
{
public:
    EchoRequestHandlerFactory(std::map<std::string, std::string> args);
    RequestHandler* create();

};

#endif