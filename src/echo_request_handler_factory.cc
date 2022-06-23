#include "echo_request_handler_factory.h"
#include "echo_request_handler.h"

EchoRequestHandlerFactory::EchoRequestHandlerFactory(std::map<std::string, std::string> args)
: RequestHandlerFactory(args)
{}

RequestHandler* EchoRequestHandlerFactory::create()
{
    return new EchoRequestHandler();
}