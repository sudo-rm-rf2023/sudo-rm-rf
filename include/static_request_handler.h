#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include "request_handler.h"

class StaticRequestHandler : public RequestHandler {
    public:
        StaticRequestHandler(const std::string base_dir);
        ~StaticRequestHandler();

        int handle_request(
        const boost::beast::http::request<boost::beast::http::string_body>& request,
        boost::beast::http::response<boost::beast::http::string_body>& response) override;

    private:
        std::string mime_type(const std::string& path);
        std::string base_dir_;
};

#endif