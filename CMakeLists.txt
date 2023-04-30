cmake_minimum_required(VERSION 3.10.0)
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

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log log_setup)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# Add the new logger source file
add_library(logger_lib src/logger.cc)

# Update name and srcs
add_library(server_lib src/server.cc)
add_library(session_lib src/session.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(config_manager_lib src/config_manager.cc)
add_library(echo_handler_lib src/echo_request_handler.cc)
add_library(static_handler_lib src/static_request_handler.cc)
add_library(router_lib src/router.cc)

target_link_libraries(config_manager_lib config_parser_lib)
target_link_libraries(server_lib config_manager_lib session_lib logger_lib)
target_link_libraries(session_lib echo_handler_lib static_handler_lib)
target_link_libraries(router_lib echo_handler_lib static_handler_lib)

# Link Boost libraries
target_link_libraries(logger_lib Boost::log Boost::log_setup Boost::system)
target_link_libraries(server_lib Boost::log Boost::log_setup)

add_executable(server src/server_main.cc)
target_link_libraries(server Boost::system server_lib session_lib echo_handler_lib)

# config parser test
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib gtest_main)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# config manager test
add_executable(config_manager_test tests/config_manager_test.cc)
target_link_libraries(config_manager_test config_manager_lib gtest_main)
gtest_discover_tests(config_manager_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# server test
add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test server_lib gtest_main)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# integration test
add_test(NAME integration_test COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration.sh)

# echo_request_handler test 
add_executable(echo_request_handler_test tests/echo_request_handler_test.cc)
target_link_libraries(echo_request_handler_test echo_handler_lib gtest_main)
gtest_discover_tests(echo_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# static_handler test
add_executable(static_request_handler_test tests/static_request_handler_test.cc)
target_link_libraries(static_request_handler_test static_handler_lib gtest_main)
gtest_discover_tests(static_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# session test
add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session_lib gtest_main)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# router test
add_executable(router_test tests/router_test.cc)
target_link_libraries(router_test router_lib gtest_main)
gtest_discover_tests(router_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# TODO(!): Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(
    TARGETS 
        server server_lib session_lib config_manager_lib config_parser_lib echo_handler_lib static_handler_lib router_lib
    TESTS 
        config_parser_test config_manager_test server_test session_test echo_request_handler_test static_request_handler_test router_test)

