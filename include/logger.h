#ifndef LOGGER_h
#define LOGGER_h

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/signals2.hpp>
#include <boost/beast/http.hpp>
namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;

class Logger
{
public:
    src::severity_logger<severity_level> lg;
    Logger();
    void init();

    void logTraceMSG(std::string trace_msg);
    void logDebugMSG(std::string debug_msg);
    void logInfoMSG(std::string info_msg);
    void logWarningMSG(std::string warning_msg);
    void logErrorMSG(std::string error_msg);
    void logFatalMSG(std::string fatal_msg);
    
    void logRequest(http::request<http::string_body> req, std::string ip_address);
    static Logger *current_logger;

    static Logger *getLogger();
};
#endif