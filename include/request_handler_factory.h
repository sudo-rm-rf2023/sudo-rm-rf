#ifndef REQUEST_HANDLER_FACTORY_H
#define REQUEST_HANDLER_FACTORY_H

#include "config_parser.h"
#include <string>

class RequestHandlerFactory {
public:
    RequestHandlerFactory(const std::string &location, const NginxConfig &config_block)
        : location_(location), config_block_(config_block) {}

protected:
    std::string location_;
    NginxConfig config_block_;
};

#endif