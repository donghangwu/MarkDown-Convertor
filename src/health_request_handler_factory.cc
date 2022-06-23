#include "health_request_handler_factory.h"
#include "health_request_handler.h"

HealthRequestHandlerFactory::HealthRequestHandlerFactory(std::map<std::string, std::string> args)
    : RequestHandlerFactory(args)
{
}

RequestHandler *HealthRequestHandlerFactory::create()
{
    return new HealthRequestHandler();
}