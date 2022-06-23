#include "request_handler_factory.h"

RequestHandlerFactory::RequestHandlerFactory(std::map<std::string, std::string> args)
{
    m_args = args;
}