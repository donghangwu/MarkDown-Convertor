#include "static_request_handler_factory.h"
#include "static_request_handler.h"

StaticRequestHandlerFactory::StaticRequestHandlerFactory(std::map<std::string, std::string> args)
: RequestHandlerFactory(args)
{}

RequestHandler* StaticRequestHandlerFactory::create()
{
    return new StaticRequestHandler(m_args["fs_path"], m_args["url_path"]);
}
