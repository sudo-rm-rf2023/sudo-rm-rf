#ifndef BLOCK_REQUEST_HANDLER_H
#define BLOCK_REQUEST_HANDLER_H

#include "request_handler.h"
#include "config_parser.h"
#include "request_handler_factory.h"
#include <optional>
#include <string>

namespace http = boost::beast::http;

class BlockRequestHandler : public RequestHandler {
    public:
        BlockRequestHandler(const std::string& location, const NginxConfig& config_block);
        status handle_request(
        const http::request<http::string_body>& request,
        http::response<http::string_body>& response) override;
    private:
        int block_time_;
};

class BlockRequestHandlerFactory : public RequestHandlerFactory {
public:
    BlockRequestHandlerFactory(const std::string &location, const NginxConfig &config_block)
        : RequestHandlerFactory(location, config_block) {}
    std::shared_ptr<RequestHandler> create() {
        return std::make_shared<BlockRequestHandler>(location_, config_block_);
    }
};

#endif