#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "config_parser.h"

const std::string CONFIG_KW_SERVER = std::string("server");
const std::string CONFIG_KW_LISTEN = std::string("listen");
const std::string CONFIG_KW_LOCATION = std::string("location");
const std::string CONFIG_KW_SERVETYPE = std::string("serveType");
const std::string CONFIG_KW_BASEDIR = std::string("baseDir");
const std::string CONFIG_KW_ECHO = std::string("echo");
const std::string CONFIG_KW_STATIC = std::string("static");
const std::string CONFIG_KW_ROOT = std::string("root");

typedef bool status;

enum HandlerType {
    UNDEFINED_HANDLER = 0,
    ECHO_HANDLER = 1,
    STATIC_HANDLER = 2,
    NOTFOUND_HANDLER = 3,
    CRUD_API_HANDLER = 4,
    HEALTH_REQUEST_HANDLER = 5
};

struct RouterEntry { // Deprecated
    std::string request_path;
    std::string base_dir; //TODO: change this to optional
    HandlerType handler_type;
};

struct DispatcherEntry {
    std::string location;
    HandlerType handler_type;
    NginxConfig location_config;
};


// TODO: Test the functions in util_test.cc
// A helper function that returns the corresponding mime_type given the request_path
std::string mime_type(const std::string& path);

#endif