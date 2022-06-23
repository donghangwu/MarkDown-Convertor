#include "server.h"
#include <string>
#include <sstream>
#include <fstream>
#include "../include/markdown/markdown.h"
using boost::asio::ip::tcp;

void termination_handler(int s)
{
  Logger *current_logger = Logger::getLogger();
  current_logger->logInfoMSG("Exit with ^C.");
  exit(1);
}

int main(int argc, char *argv[])
{
  // std::ifstream file;
  // std::stringstream markdownInput, htmlout;
  // file.open("myFile");
  // markdownInput << file.rdbuf();

  // // std::ifstream ifile;

  // std::istream *in = &markdownInput;
  // markdown::Document doc;
  // doc.read(markdownInput);
  // doc.write(htmlout);
  // std::cout << htmlout.str();
  // file.close();

  // return 0;

  Logger *current_logger = Logger::getLogger();
  try
  {
    struct sigaction signal_action;
    signal_action.sa_handler = termination_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    sigaction(SIGINT, &signal_action, NULL);

    if (argc != 2)
    {
      current_logger->logErrorMSG("Invalid arguments. Exit.");
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }
    // parse the config file.
    NginxConfig my_config;
    NginxConfigParser my_config_parser;
    current_logger->logTraceMSG("Parsing the config file.");
    if (my_config_parser.Parse(argv[1], &my_config) == false)
    {
      current_logger->logErrorMSG("Wrong file. Exit.");
      std::cerr << "Wrong config file\n";
      return 2;
    }
    int port_num = my_config.GetPortNum();
    if (port_num < 0)
    {
      current_logger->logErrorMSG("Invalid port number. Exit.");
      return 3; // Arbitrary error code, for debug purposes
    }
    boost::asio::io_service io_service;

    using namespace std;

    current_logger->logInfoMSG("Starting server on port " + to_string(port_num));
    server s(io_service, my_config);
    s.start_accept();
    s.run();
  }

  catch (std::exception &e)
  {
    current_logger->logFatalMSG("Exception in server_main.cc. Exit.");
    std::cerr << "Exception in server_main.cc : " << e.what() << "\n";
    current_logger->logFatalMSG(e.what());
  }

  return 0;
}
