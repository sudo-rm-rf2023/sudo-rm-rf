#include "echo_request_handler.h"
#include <boost/beast/http/verb.hpp>
#include <sstream>

#include <boost/beast.hpp>
#include <sstream>
#include <string>

std::string request_to_string(const boost::beast::http::request<boost::beast::http::string_body> &request) {
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
    for (const auto& field : request) {
        header_stream << field.name_string() << ": " << field.value() << "\r\n";
    }
    header_stream << "\r\n";

    // Get the header as a string
    std::string header_str = header_stream.str();

    // Concatenate the header and body, and store in a std::string object
    std::string full_request_str = header_str + request.body();

    return full_request_str;
}



int EchoRequestHandler::handle_request(
    const boost::beast::http::request<boost::beast::http::string_body>& request, 
    boost::beast::http::response<boost::beast::http::string_body>& response) {

    response.version(request.version());
    response.result(boost::beast::http::status::ok);

    response.body() = request_to_string(request);
    response.set(boost::beast::http::field::content_type, "text/plain");
    response.prepare_payload(); // set content-length
    printf("Response Generated.\n");
    return 1;
}

int EchoRequestHandler::handle_bad_request(boost::beast::http::response<boost::beast::http::string_body>& response) {
    response.result(boost::beast::http::status::bad_request);
    response.set(boost::beast::http::field::content_type, "text/plain");
    response.body() = "Invalid Request";
    response.prepare_payload();
    return 1;
}