#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include "config_parser.h"
#include "request_handler_factory.h"

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

class NotFoundHandler : public RequestHandler {
public:
    NotFoundHandler(const std::string &location, const NginxConfig &config_block) {}

    int handle_request(
        const http::request<http::string_body> &request,
        http::response<http::string_body> &response) override;
};

// Factory
class NotFoundHandlerFactory : public RequestHandlerFactory {
public:
    NotFoundHandlerFactory(const std::string &location, const NginxConfig &config_block)
        : RequestHandlerFactory(location, config_block) {}
    std::shared_ptr<RequestHandler> create() {
        return std::make_shared<NotFoundHandler>(location_, config_block_);
    }
};

#endif