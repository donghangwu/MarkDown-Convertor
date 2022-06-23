#ifndef NOT_FOUND_REQUEST_HANDLER_FACTORY_H
#define NOT_FOUND_REQUEST_HANDLER_FACTORY_H

#include <string>
#include <map>
#include "request_handler.h"
#include "request_handler_factory.h"

class Not_Found_RequestHandlerFactory : public RequestHandlerFactory
{
public:
    Not_Found_RequestHandlerFactory(std::map<std::string, std::string> args);
    RequestHandler *create();
};

#endif