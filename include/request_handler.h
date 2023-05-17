#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "utils.h"
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body.hpp>
#include <cstdlib>
#include <optional>
#include <string>

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

class RequestHandler {
public:
    struct Options { // Deprecated
        std::string request_path;
        std::optional<std::string> base_dir;
    };

    virtual status handle_request(
        const http::request<http::string_body> &request,
        http::response<http::string_body> &response) = 0;

    status handle_bad_request(http::response<http::string_body> &response) {
        response.result(http::status::bad_request);
        response.set(http::field::content_type, "text/plain");
        response.body() = "Invalid Request";
        response.prepare_payload();
        return false;
    }
};

#endif