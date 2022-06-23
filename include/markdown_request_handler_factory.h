#ifndef MARKDOWN_REQUEST_HANDLER_FACTORY_H
#define MARKDOWN_REQUEST_HANDLER_FACTORY_H

#include <string>
#include <map>
#include "request_handler.h"
#include "request_handler_factory.h"

class Markdown_RequestHandlerFactory : public RequestHandlerFactory
{
public:
    Markdown_RequestHandlerFactory(std::map<std::string, std::string> args);
    RequestHandler *create();
};

#endif