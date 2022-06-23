cmake_minimum_required(VERSION 3.10.0)
project(snore)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS log_setup log system filesystem regex)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

#: Update name and srcs
add_library(logger src/logger.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(server_lib src/server.cc src/crud_request_handler.cc src/crud_request_handler_factory.cc src/fs_wrapper.cc src/echo_request_handler.cc src/not_found_request_handler.cc src/static_request_handler.cc src/request_handler_factory.cc src/static_request_handler_factory.cc src/echo_request_handler_factory.cc src/not_found_request_handler_factory.cc src/markdown_request_handler_factory.cc src/markdown_request_handler.cc src/request_handler_factory.cc)
add_library(session_lib src/session.cc src/echo_request_handler.cc src/not_found_request_handler.cc src/static_request_handler.cc src/static_request_handler_factory.cc src/echo_request_handler_factory.cc src/request_handler_factory.cc src/not_found_request_handler_factory.cc src/markdown_request_handler_factory.cc src/markdown_request_handler.cc src/request_handler_factory.cc)
add_library(echo_lib src/echo_request_handler.cc)
add_library(static_lib src/not_found_request_handler.cc src/static_request_handler.cc)
add_library(req_fact_lib src/request_handler_factory.cc)
add_library(echo_fact_lib src/echo_request_handler_factory.cc src/echo_request_handler.cc src/request_handler_factory.cc)
add_library(static_fact_lib src/static_request_handler_factory.cc src/static_request_handler.cc src/request_handler_factory.cc)
add_library(not_found_lib src/not_found_request_handler.cc src/static_request_handler.cc src/request_handler_factory.cc src/not_found_request_handler_factory.cc )

add_library(health_lib src/health_request_handler.cc src/request_handler_factory.cc )
add_library(health_fact_lib src/health_request_handler_factory.cc src/health_request_handler.cc src/request_handler_factory.cc )

add_library(sleep_lib src/sleep_request_handler.cc src/request_handler_factory.cc )
add_library(sleep_fact_lib src/sleep_request_handler_factory.cc src/sleep_request_handler.cc src/request_handler_factory.cc )

add_library(markdown_lib include/markdown/markdown.cpp include/markdown/markdown-tokens.cpp )

add_library(markdown_req_lib src/markdown_request_handler.cc src/request_handler_factory.cc include/markdown/markdown.cpp include/markdown/markdown-tokens.cpp)
add_library(markdown_fact_lib src/markdown_request_handler_factory.cc src/markdown_request_handler.cc src/request_handler_factory.cc )


#: Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
target_link_libraries(server Boost::system pthread config_parser_lib server_lib session_lib static_lib logger req_fact_lib echo_fact_lib static_fact_lib health_lib health_fact_lib sleep_lib sleep_fact_lib markdown_req_lib markdown_fact_lib markdown_lib Boost::system Boost::log_setup Boost::log Boost::filesystem Boost::regex)


#: Update test executable name, srcs, and deps
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib gtest_main)

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test  server_lib session_lib health_lib health_fact_lib sleep_lib sleep_fact_lib gtest_main 
    config_parser_lib static_lib logger markdown_lib markdown_req_lib Boost::system Boost::log_setup 
    Boost::log Boost::filesystem Boost::regex)


add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session_lib health_lib health_fact_lib gtest_main markdown_lib markdown_req_lib logger 
    Boost::system Boost::log_setup Boost::log Boost::filesystem Boost::regex)

add_executable(static_test tests/static_test.cc)
target_link_libraries(static_test static_lib gtest_main)

add_executable(echo_test tests/echo_test.cc)
target_link_libraries(echo_test echo_lib gtest_main)

add_executable(not_found_test tests/not_found_test.cc)
target_link_libraries(not_found_test not_found_lib gtest_main)

add_executable(logger_test tests/logger_test.cc)
target_link_libraries(logger_test gtest_main Boost::system Boost::filesystem logger 
                       Boost::regex Boost::log_setup Boost::log)

add_executable(crud_request_test tests/crud_request_test.cc)
target_link_libraries(crud_request_test server_lib logger Boost::log_setup Boost::log Boost::filesystem gtest_main gmock_main)

add_executable(health_request_test tests/health_test.cc)
target_link_libraries(health_request_test health_lib gtest_main)


add_executable(sleep_request_test tests/sleep_test.cc)
target_link_libraries(sleep_request_test sleep_lib gtest_main)

add_executable(markdown_request_test tests/markdown_test.cc)
target_link_libraries(markdown_request_test markdown_req_lib  gtest_main Boost::regex)

#: Update with test binary
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(static_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(crud_request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(not_found_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(logger_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(health_request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(sleep_request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(markdown_request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Integration tests
add_test(integration_test /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh ${CMAKE_CURRENT_SOURCE_DIR}/build/bin/server ${CMAKE_CURRENT_SOURCE_DIR}/config_files/ex_config ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests_echo echo)
add_test(integration_test /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh ${CMAKE_CURRENT_SOURCE_DIR}/build/bin/server ${CMAKE_CURRENT_SOURCE_DIR}/config_files/ex_config ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests_static static)
add_test(integration_test /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh ${CMAKE_CURRENT_SOURCE_DIR}/build/bin/server ${CMAKE_CURRENT_SOURCE_DIR}/config_files/ex_config ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests_sleep sleep)
add_test(integration_test /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh ${CMAKE_CURRENT_SOURCE_DIR}/build/bin/server ${CMAKE_CURRENT_SOURCE_DIR}/config_files/ex_config ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests_health health)
#: Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS config_parser_lib session_lib server_lib static_lib echo_lib not_found_lib logger health_lib sleep_lib markdown_req_lib TESTS config_parser_test server_test session_test static_test echo_test crud_request_test not_found_test logger_test health_request_test sleep_request_test markdown_request_test)