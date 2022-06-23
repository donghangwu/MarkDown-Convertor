#include "sleep_request_handler_factory.h"
#include "sleep_request_handler.h"

SleepRequestHandlerFactory::SleepRequestHandlerFactory(std::map<std::string, std::string> args)
    : RequestHandlerFactory(args)
{
}

RequestHandler *SleepRequestHandlerFactory::create()
{
    return new SleepRequestHandler();
}