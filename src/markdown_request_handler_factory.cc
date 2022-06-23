#include "markdown_request_handler_factory.h"
#include "markdown_request_handler.h"

Markdown_RequestHandlerFactory::Markdown_RequestHandlerFactory(std::map<std::string, std::string> args)
    : RequestHandlerFactory(args)
{
}

RequestHandler *Markdown_RequestHandlerFactory::create()
{
    return new MarkdownRequestHandler();
}