#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include "config_parser.h"
#include "request_handler_factory.h"

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

class EchoRequestHandler : public RequestHandler {
public:
    EchoRequestHandler(const std::string &location, const NginxConfig &config_block);
    static EchoRequestHandler *makeEchoRequestHandler(const RequestHandler::Options &options); // Deprecated

    status handle_request(
        const http::request<http::string_body> &request,
        http::response<http::string_body> &response) override;

private:
    EchoRequestHandler(const RequestHandler::Options &options); // Deprecated
    std::string request_path_;
};

// Factory
class EchoHandlerFactory : public RequestHandlerFactory {
public:
    EchoHandlerFactory(const std::string &location, const NginxConfig &config_block)
        : RequestHandlerFactory(location, config_block) {}
    std::shared_ptr<RequestHandler> create() {
        return std::make_shared<EchoRequestHandler>(location_, config_block_);
    }
};

#endif