#include "crud_request_handler_factory.h"

#include <boost/filesystem.hpp>

#include "crud_request_handler.h"

CrudRequestHandlerFactory::CrudRequestHandlerFactory(
    std::map<std::string, std::string> args)
    : RequestHandlerFactory(args) {
  factory_logger = Logger::getLogger();
  initializeDirectory(args["fs_path"]);
}

RequestHandler* CrudRequestHandlerFactory::create() {
  return new CrudRequestHandler(m_args["fs_path"], m_args["url_path"], nullptr, nullptr);
}

void CrudRequestHandlerFactory::initializeDirectory(std::string dir) {
  factory_logger->logInfoMSG("Initializing directory '" + dir + "'");

  // Create folder if it doesn't exist
  if (!boost::filesystem::is_directory(dir)) {
    // If folder does not exist and cannot be created, fatal error
    if (!boost::filesystem::create_directory(dir)) {
      factory_logger->logFatalMSG("Unable to create directory '" + dir + "'");
      exit(EXIT_FAILURE);
    }
  } else {
    factory_logger->logInfoMSG("Directory '" + dir + "' already exists");
  }
}