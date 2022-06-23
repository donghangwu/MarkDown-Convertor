#ifndef HEALTH_REQUEST_HANDLER_FACTORY_H
#define HEALTH_REQUEST_HANDLER_FACTORY_H

#include <string>
#include <map>
#include "request_handler.h"
#include "request_handler_factory.h"

class HealthRequestHandlerFactory : public RequestHandlerFactory
{
public:
    HealthRequestHandlerFactory(std::map<std::string, std::string> args);
    RequestHandler *create();
};

#endif