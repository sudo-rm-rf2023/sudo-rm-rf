#include "static_request_handler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "logger.h"
#include "utils.h"

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

// Constructor
StaticRequestHandler::StaticRequestHandler(const RequestHandler::Options& options){
    request_path_ = options.request_path;
    base_dir_ = options.base_dir.value_or(std::string());
    BOOST_LOG_TRIVIAL(info) << "StaticRequestHandler created. Base directory:" << base_dir_;
}

// Factory Method
StaticRequestHandler* StaticRequestHandler::makeStaticRequestHandler(const RequestHandler::Options& options){
    if (options.base_dir.has_value()) {
        return new StaticRequestHandler(options);
    } else { //invalid options: StaticRequestHandler requires a base_dir.
        return nullptr;
    }
}

int StaticRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {

    response.version(request.version());
    response.result(http::status::ok);

    // Build file path from request target
    int offset = request_path_.length();
    std::string requested_file = request.target().to_string().substr(offset);
    std::string file_path = base_dir_ + requested_file;
    BOOST_LOG_TRIVIAL(info) << "StaticRequestHandler::handle_request called";
    BOOST_LOG_TRIVIAL(info) << "Request method: " << request.method_string();
    BOOST_LOG_TRIVIAL(info) << "file_path: " << file_path;

    // Check if file exists
    std::ifstream file(file_path, std::ios::in | std::ios::binary);
    // TODO: Check if file is a plain file (not a direcory, soft link, hard link etc).
    if (!file) {
        BOOST_LOG_TRIVIAL(info) << "File not found";
        response.result(http::status::not_found);
        response.set(http::field::content_type, "text/plain");
        response.body() = "File not found";
    } else {
        BOOST_LOG_TRIVIAL(info) << "File found";
        // Read file content into response body
        std::ostringstream content_stream;
        content_stream << file.rdbuf();
        response.body() = content_stream.str();
        response.content_length(response.body().size());

        // Set content type and status
        response.set(http::field::content_type, mime_type(file_path));
        response.result(http::status::ok);
    }

    // Prepare the response
    response.prepare_payload();

    return 0; // Return 0 to indicate success
    }
