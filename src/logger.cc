#include "logger.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;

Logger *Logger::current_logger = 0;

Logger *Logger::getLogger()
{
    if (Logger::current_logger == 0)
        Logger::current_logger = new Logger();
    return Logger::current_logger;
}

Logger::Logger()
{
    init();
    logging::add_common_attributes();
    logging::record rec = lg.open_record();
    logging::add_console_log(std::cout, boost::log::keywords::format = ">> %Message%");
}
// From Discussion slides
void Logger::init()
{
    logging::add_file_log(
        keywords::file_name = "../log/SYSLOG_%N.log",                                 // Choose file name pattern - SYSLOG_0.log, SYSLOG_1.log
        keywords::rotation_size = 10 * 1024 * 1024,                                   // Rotate files for every  10 MB (value given in Bytes)
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), // Or rotate at Midnight
        keywords::format = "[%TimeStamp%]:[%ThreadID%]:%Message%",                    // Log record format
        keywords::auto_flush = true                                                   // allows boost to log after each logging code line is executed
    );
}



void Logger::logTraceMSG(std::string trace_msg)
{
    BOOST_LOG_SEV(lg, trace) << "Trace: " << trace_msg;
}

void Logger::logDebugMSG(std::string debug_msg)
{
    BOOST_LOG_SEV(lg, debug) << "Debug: " << debug_msg;
}

void Logger::logInfoMSG(std::string info_msg)
{
    BOOST_LOG_SEV(lg, info) << "Info: " << info_msg;
}

void Logger::logRequest(http::request<http::string_body> req, std::string ip_address)
{
    std::string info_msg;
    std::stringstream buf;
    buf << req.method_string() << " " << req.target() << " " << req.version() << " Client IP: " << ip_address;
    info_msg = buf.str();
    BOOST_LOG_SEV(lg, info) << "Info: " << info_msg;
}

void Logger::logWarningMSG(std::string warning_msg)
{
    BOOST_LOG_SEV(lg, warning) << "Warning: " << warning_msg;
}

void Logger::logErrorMSG(std::string error_msg)
{
    BOOST_LOG_SEV(lg, error) << "Error: " << error_msg;
}

void Logger::logFatalMSG(std::string fatal_msg)
{
    BOOST_LOG_SEV(lg, fatal) << "Fatal: " << fatal_msg;
}


