#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include "request_handler.h"
#include "utils.h"

class EchoRequestHandler : public RequestHandler {
    public:
        static EchoRequestHandler* makeEchoRequestHandler(const RequestHandler::Options& options);

        int handle_request(
        const boost::beast::http::request<boost::beast::http::string_body>& request,
        boost::beast::http::response<boost::beast::http::string_body>& response) override;
    private:
        EchoRequestHandler(const RequestHandler::Options& options);
        std::string request_path_;
};

#endif