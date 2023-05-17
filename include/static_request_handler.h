#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include "request_handler.h"
#include "config_parser.h"
#include "request_handler_factory.h"
#include <optional>
#include <string>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class StaticRequestHandler : public RequestHandler {
    public:
        StaticRequestHandler(const std::string& location, const NginxConfig& config_block);
        static StaticRequestHandler* makeStaticRequestHandler(const RequestHandler::Options& options); // Deprecated

        status handle_request(
        const http::request<http::string_body>& request,
        http::response<http::string_body>& response) override;
    private:
        StaticRequestHandler(const RequestHandler::Options& options); //Deprecated
        std::string request_path_;
        std::optional<std::string> base_dir_ = std::nullopt;
};

// Factory
class StaticHandlerFactory : public RequestHandlerFactory {
public:
    StaticHandlerFactory(const std::string &location, const NginxConfig &config_block)
        : RequestHandlerFactory(location, config_block) {}
    std::shared_ptr<RequestHandler> create() {
        return std::make_shared<StaticRequestHandler>(location_, config_block_);
    }
};

#endif