#ifndef SLEEP_REQUEST_HANDLER_FACTORY_H
#define SLEEP_REQUEST_HANDLER_FACTORY_H

#include <string>
#include <map>
#include "request_handler.h"
#include "request_handler_factory.h"

class SleepRequestHandlerFactory : public RequestHandlerFactory
{
public:
    SleepRequestHandlerFactory(std::map<std::string, std::string> args);
    RequestHandler *create();
};

#endif