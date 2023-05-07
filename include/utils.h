#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>

const std::string CONFIG_KW_SERVER = std::string("server");
const std::string CONFIG_KW_LISTEN = std::string("listen");
const std::string CONFIG_KW_LOCATION = std::string("location");
const std::string CONFIG_KW_SERVETYPE = std::string("serveType");
const std::string CONFIG_KW_BASEDIR = std::string("baseDir");
const std::string CONFIG_KW_ECHO = std::string("echo");
const std::string CONFIG_KW_STATIC = std::string("static");

enum HandlerType {
    ECHO_HANDLER = 1,
    STATIC_HANDLER = 2,
};

struct RouterEntry {
    std::string request_path;
    std::string base_dir; //TODO: change this to optional
    HandlerType handler_type;
};

// TODO: Test the functions in util_test.cc
// A helper function that returns the corresponding mime_type given the request_path
std::string mime_type(const std::string& path);

#endif