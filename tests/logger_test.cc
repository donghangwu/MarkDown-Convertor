#include "gtest/gtest.h"
#include "logger.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class LoggerTestFixture : public ::testing::Test
{
protected:
    Logger * current_logger = Logger::getLogger();
    ifstream myfile;
    string file_content;
    const int START_POS_MSG = 50;
    const string message = "Hello world!";
    void readFile()
    {
        myfile.open ("../log/SYSLOG_0.log");
        getline(myfile,file_content);
        myfile.close();
    }
};

TEST_F(LoggerTestFixture, test_trace)
{
    current_logger -> logTraceMSG(message);
    readFile();
    EXPECT_TRUE(file_content.substr(START_POS_MSG) == "Trace: " + message);
}

TEST_F(LoggerTestFixture, test_debug)
{
    current_logger -> logDebugMSG(message);
    readFile();
    EXPECT_TRUE(file_content.substr(START_POS_MSG) == "Debug: " + message);
}

TEST_F(LoggerTestFixture, test_info)
{
    current_logger -> logInfoMSG(message);
    readFile();
    EXPECT_TRUE(file_content.substr(START_POS_MSG) == "Info: " + message);
}

TEST_F(LoggerTestFixture, test_warning)
{
    current_logger -> logWarningMSG(message);
    readFile();
    EXPECT_TRUE(file_content.substr(START_POS_MSG) == "Warning: " + message);
}

TEST_F(LoggerTestFixture, test_error)
{
    current_logger -> logErrorMSG(message);
    readFile();
    EXPECT_TRUE(file_content.substr(START_POS_MSG) == "Error: " + message);
}

TEST_F(LoggerTestFixture, test_fatal)
{
    current_logger -> logFatalMSG(message);
    readFile();
    EXPECT_TRUE(file_content.substr(START_POS_MSG) == "Fatal: " + message);
}