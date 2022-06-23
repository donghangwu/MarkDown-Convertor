#ifndef CRUD_REQUEST_HANDLER_FACTORY_H
#define CRUD_REQUEST_HANDLER_FACTORY_H

#include <map>
#include <string>

#include "logger.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class CrudRequestHandlerFactory : public RequestHandlerFactory {
 public:
  CrudRequestHandlerFactory(std::map<std::string, std::string> args);
  RequestHandler* create();
  Logger* factory_logger;

 private:
  void initializeDirectory(std::string);
};

#endif