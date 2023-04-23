

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
    header_stream << request.method_string() << " " << request.target() << " HTTP/" << major_version << "." << minor_version << "\r\n";
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
    response.set(boost::beast::http::field::content_type, "text/html");
    response.prepare_payload(); // set content-length
    printf("Response Generated.\n");
    return 1;
}