#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include "request_handler.h"

class StaticRequestHandler : public RequestHandler {
    public:
        static StaticRequestHandler* makeStaticRequestHandler(const RequestHandler::Options& options);

        int handle_request(
        const boost::beast::http::request<boost::beast::http::string_body>& request,
        boost::beast::http::response<boost::beast::http::string_body>& response) override;
    private:
        StaticRequestHandler(const RequestHandler::Options& options);
        std::string request_path_;
        std::string base_dir_;
};

#endif