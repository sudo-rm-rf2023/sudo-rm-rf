#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include "request_handler.h"
#include "utils.h"

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class EchoRequestHandler : public RequestHandler {
    public:
        static EchoRequestHandler* makeEchoRequestHandler(const RequestHandler::Options& options);

        int handle_request(
        const http::request<http::string_body>& request,
        http::response<http::string_body>& response) override;
    private:
        EchoRequestHandler(const RequestHandler::Options& options);
        std::string request_path_;
};

#endif