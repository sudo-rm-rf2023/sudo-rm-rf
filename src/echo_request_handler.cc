#include "echo_request_handler.h"
#include "config_parser.h"
#include <boost/beast/http/verb.hpp>
#include <sstream>

#include "logger.h"
#include <boost/beast.hpp>
#include <sstream>
#include <string>

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

// Constructor
EchoRequestHandler::EchoRequestHandler(const std::string &location, const NginxConfig &config_block) {
    request_path_ = location;
}

std::string request_to_string(const http::request<http::string_body> &request) {
    // Create a std::ostringstream to store the header
    std::ostringstream header_stream;

    // Calculate major and minor version numbers
    int major_version = request.version() / 10;
    int minor_version = request.version() % 10;

    // Write the header to the stream
    header_stream << request.method_string();
    header_stream << " ";
    header_stream << request.target();
    header_stream << " HTTP/";
    header_stream << major_version;
    header_stream << ".";
    header_stream << minor_version;
    header_stream << "\r\n";
    for (const auto &field : request) {
        header_stream << field.name_string() << ": " << field.value() << "\r\n";
    }
    header_stream << "\r\n";

    // Get the header as a string
    std::string header_str = header_stream.str();
    BOOST_LOG_TRIVIAL(info) << "Header: " << header_str;

    // Concatenate the header and body, and store in a std::string object
    std::string full_request_str = header_str + request.body();

    return full_request_str;
}

EchoRequestHandler::EchoRequestHandler(const RequestHandler::Options &options) {
    request_path_ = options.request_path;
}

EchoRequestHandler *EchoRequestHandler::makeEchoRequestHandler(const RequestHandler::Options &options) {
    return new EchoRequestHandler(options);
}

status EchoRequestHandler::handle_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {

    BOOST_LOG_TRIVIAL(info) << MATCHED_HANDLER << "EchoRequestHandler";

    response.version(request.version());
    response.result(http::status::ok);

    BOOST_LOG_TRIVIAL(info) << RESPONSE_CODE << response.result_int();

    response.body() = request_to_string(request);
    response.set(http::field::content_type, "text/plain");
    response.prepare_payload(); // set content-length

    return true;
}
