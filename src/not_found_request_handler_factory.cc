#include "not_found_request_handler_factory.h"
#include "not_found_request_handler.h"

Not_Found_RequestHandlerFactory::Not_Found_RequestHandlerFactory(std::map<std::string, std::string> args)
    : RequestHandlerFactory(args)
{
}

RequestHandler *Not_Found_RequestHandlerFactory::create()
{
    return new NotFoundRequestHandler();
}