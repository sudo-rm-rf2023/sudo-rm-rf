#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>

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