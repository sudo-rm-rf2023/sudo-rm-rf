#include "health_request_handler.h"
#include "logger.h"

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

status HealthRequestHandler::handle_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {

    BOOST_LOG_TRIVIAL(info) << "HealthFoundHandler::handle_request called";

    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::content_type, "text/plain");
    response.body() = "OK";
    response.prepare_payload();
    return true;
}