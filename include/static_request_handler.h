#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include "request_handler.h"

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class StaticRequestHandler : public RequestHandler {
    public:
        static StaticRequestHandler* makeStaticRequestHandler(const RequestHandler::Options& options);

        int handle_request(
        const http::request<http::string_body>& request,
        http::response<http::string_body>& response) override;
    private:
        StaticRequestHandler(const RequestHandler::Options& options);
        std::string request_path_;
        std::string base_dir_;
};

#endif