#ifndef HEALTH_REQUEST_HANDLER_H
#define HEALTH_REQUEST_HANDLER_H

#include "request_handler.h"
#include "config_parser.h"
#include "request_handler_factory.h"
#include <string>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class HealthRequestHandler : public RequestHandler {
    public:
        HealthRequestHandler(const std::string& location, const NginxConfig& config_block) {}

        status handle_request(
        const http::request<http::string_body>& request,
        http::response<http::string_body>& response) override;
};

// Factory
class HealthRequestHandlerFactory : public RequestHandlerFactory {
public:
    HealthRequestHandlerFactory(const std::string &location, const NginxConfig &config_block)
        : RequestHandlerFactory(location, config_block) {}
    std::shared_ptr<RequestHandler> create() {
        return std::make_shared<HealthRequestHandler>(location_, config_block_);
    }
};

#endif