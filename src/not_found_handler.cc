#include "not_found_handler.h"
#include "logger.h"
#include <boost/beast.hpp>
#include <boost/beast/http/verb.hpp>
#include <string>

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

int NotFoundHandler::handle_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {

    BOOST_LOG_TRIVIAL(info) << "NotFoundHandler::handle_request called";

    response.version(request.version());
    response.result(http::status::not_found);
    response.set(http::field::content_type, "text/plain");
    response.body() = "Not Found";
    response.prepare_payload();
    return 0;
}
