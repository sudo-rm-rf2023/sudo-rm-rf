cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
project(sudo-rm-rf)

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

# Enable JsonCpp
find_package(jsoncpp)

# Enable OpenSSL
find_package(OpenSSL REQUIRED)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log log_setup)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include legacy/include)

# Add the new logger source file
add_library(logger_lib src/logger.cc)

# Libraries and source file
add_library(server_lib src/server.cc)
add_library(session_lib src/session.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(echo_handler_lib src/echo_request_handler.cc)
add_library(static_handler_lib src/static_request_handler.cc)
add_library(not_found_handler_lib src/not_found_handler.cc)
add_library(crud_api_handler_lib src/crud_api_handler.cc)
add_library(health_request_handler_lib src/health_request_handler.cc)
add_library(block_request_handler_lib src/block_request_handler.cc)
add_library(utils_lib src/utils.cc)
add_library(config_utils_lib src/config_utils.cc)
add_library(dispatcher_lib src/dispatcher.cc)
add_library(boost_file_system_io_lib src/boost_file_system_io.cc)
add_library(score_util_lib src/score_util.cc)
add_library(test_file_system_io_lib src/test_file_system_io.cc)
add_library(default_score_manager_lib src/default_score_manager.cc)
add_library(score_request_handler_lib src/score_request_handler.cc)

# Deprecated libraries
add_library(config_manager_lib legacy/src/config_manager.cc)
add_library(router_lib legacy/src/router.cc)

# Dependencies
target_link_libraries(config_manager_lib config_parser_lib config_utils_lib logger_lib)
target_link_libraries(server_lib config_manager_lib session_lib logger_lib)
target_link_libraries(session_lib echo_handler_lib static_handler_lib not_found_handler_lib router_lib dispatcher_lib logger_lib)
target_link_libraries(router_lib echo_handler_lib static_handler_lib not_found_handler_lib)
target_link_libraries(config_utils_lib config_parser_lib utils_lib logger_lib)
target_link_libraries(dispatcher_lib config_utils_lib utils_lib logger_lib static_handler_lib 
    echo_handler_lib not_found_handler_lib crud_api_handler_lib health_request_handler_lib 
    block_request_handler_lib boost_file_system_io_lib score_request_handler_lib)
target_link_libraries(logger_lib Boost::log Boost::log_setup Boost::system)
target_link_libraries(config_parser_lib logger_lib)
target_link_libraries(static_handler_lib logger_lib utils_lib config_utils_lib)
target_link_libraries(echo_handler_lib logger_lib)
target_link_libraries(not_found_handler_lib logger_lib)
target_link_libraries(crud_api_handler_lib logger_lib config_utils_lib)
target_link_libraries(health_request_handler_lib logger_lib)
target_link_libraries(block_request_handler_lib logger_lib config_utils_lib)
target_link_libraries(utils_lib config_parser_lib)
target_link_libraries(boost_file_system_io_lib logger_lib)
target_link_libraries(score_util_lib jsoncpp_lib logger_lib OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(default_score_manager_lib logger_lib score_util_lib jsoncpp)
target_link_libraries(score_request_handler_lib logger_lib default_score_manager_lib utils_lib)

# main
add_executable(server src/server_main.cc)
target_link_libraries(server Boost::system server_lib session_lib echo_handler_lib static_handler_lib not_found_handler_lib router_lib)

# config parser test
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib gtest_main gmock)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# config manager test
add_executable(config_manager_test legacy/tests/config_manager_test.cc)
target_link_libraries(config_manager_test config_manager_lib gtest_main gmock)
gtest_discover_tests(config_manager_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/legacy/tests)

# # server test
# add_executable(server_test tests/server_test.cc)
# target_link_libraries(server_test server_lib gtest_main)
# gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# dispatcher test
add_executable(dispatcher_test tests/dispatcher_test.cc)
target_link_libraries(dispatcher_test dispatcher_lib gtest_main gmock)
gtest_discover_tests(dispatcher_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# integration test
file(GLOB_RECURSE DIR_INTEGRATION_TEST
     LIST_DIRECTORIES true
     CONFIGURE_DEPENDS
     tests/integration_tests)
file(COPY ${DIR_INTEGRATION_TEST} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) # Copy the integration test directory to output dir

add_test(NAME integration_test WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/integration_tests/ COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/integration_tests/integration.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server)

# echo_request_handler test 
add_executable(echo_request_handler_test tests/echo_request_handler_test.cc)
target_link_libraries(echo_request_handler_test echo_handler_lib gtest_main)
gtest_discover_tests(echo_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# static_handler test
add_executable(static_request_handler_test tests/static_request_handler_test.cc)
target_link_libraries(static_request_handler_test static_handler_lib gtest_main)
gtest_discover_tests(static_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# not_found_handler test
add_executable(not_found_handler_test tests/not_found_handler_test.cc)
target_link_libraries(not_found_handler_test not_found_handler_lib gtest_main)
gtest_discover_tests(not_found_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# crud_api_handler test
add_executable(crud_api_handler_test tests/crud_api_handler_test.cc)
target_link_libraries(crud_api_handler_test crud_api_handler_lib gtest_main test_file_system_io_lib)
gtest_discover_tests(crud_api_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# health_request_handler test
add_executable(health_request_handler_test tests/health_request_handler_test.cc)
target_link_libraries(health_request_handler_test health_request_handler_lib gtest_main)
gtest_discover_tests(health_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# block_request_handler test
add_executable(block_request_handler_test tests/block_request_handler_test.cc)
target_link_libraries(block_request_handler_test block_request_handler_lib gtest_main)
gtest_discover_tests(block_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# session test
add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session_lib gtest_main)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# router test
add_executable(router_test legacy/tests/router_test.cc)
target_link_libraries(router_test router_lib gtest_main)
gtest_discover_tests(router_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/legacy/tests)

# config_utils test
add_executable(config_utils_test tests/config_utils_test.cc)
target_link_libraries(config_utils_test config_utils_lib gtest_main gmock)
gtest_discover_tests(config_utils_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# file_system_io test
add_executable(boost_file_system_io_test tests/boost_file_system_io_test.cc)
target_link_libraries(boost_file_system_io_test boost_file_system_io_lib gtest_main)
gtest_discover_tests(boost_file_system_io_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# utils test
add_executable(utils_test tests/utils_test.cc)
target_link_libraries(utils_test utils_lib gtest_main)
gtest_discover_tests(utils_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# score_util test
add_executable(score_util_test tests/score_util_test.cc)
target_link_libraries(score_util_test score_util_lib gtest_main)
gtest_discover_tests(score_util_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# default_score_manager test
add_executable(default_score_manager_test tests/default_score_manager_test.cc)
target_link_libraries(default_score_manager_test default_score_manager_lib gtest_main test_file_system_io_lib)
gtest_discover_tests(default_score_manager_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# score_request_handler test
add_executable(score_request_handler_test tests/score_request_handler_test.cc)
target_link_libraries(score_request_handler_test score_request_handler_lib gtest_main gmock jsoncpp_lib score_util_lib config_utils_lib boost_file_system_io_lib)

# Coverage target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(
    TARGETS 
        server server_lib session_lib config_manager_lib config_parser_lib echo_handler_lib health_request_handler_lib block_request_handler_lib
        static_handler_lib not_found_handler_lib crud_api_handler_lib router_lib config_utils_lib dispatcher_lib boost_file_system_io_lib utils_lib
        score_util_lib default_score_manager_lib
    TESTS 
        config_parser_test config_manager_test session_test echo_request_handler_test health_request_handler_test block_request_handler_test
        static_request_handler_test not_found_handler_test crud_api_handler_test router_test config_utils_test dispatcher_test boost_file_system_io_test
        utils_test score_util_test default_score_manager_test score_request_handler_test)
