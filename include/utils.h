#ifndef UTILS_H
#define UTILS_H

#include <string>

enum HandlerType {
    ECHO_HANDLER = 1,
    STATIC_HANDLER = 2,
};

struct RouterEntry {
    std::string request_target;
    std::string base_dir;
    HandlerType handler_type;
};

#endif