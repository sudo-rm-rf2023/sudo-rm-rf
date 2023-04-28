#include "static_request_handler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>


// Constructor
StaticRequestHandler::StaticRequestHandler(const std::string base_dir)
    : base_dir_(base_dir) {
    printf("StaticRequestHandler created\n");
    printf("base_dir: %s\n", base_dir_.c_str());
}

// Destructor
StaticRequestHandler::~StaticRequestHandler() {
}

std::string StaticRequestHandler::mime_type(const std::string& path) {
    // Mapping of file extensions to MIME types
    static const std::unordered_map<std::string, std::string> mime_types{
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".json", "application/json"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".gif", "image/gif"},
        {".svg", "image/svg+xml"},
        {".ico", "image/x-icon"},
        {".txt", "text/plain"},
        // Add more MIME types here as needed
    };

    // Find the last '.' in the path to determine the file extension
    std::size_t pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = path.substr(pos);
        auto it = mime_types.find(ext);
        if (it != mime_types.end()) {
            return it->second;
        }
    }

    // Default MIME type if the file extension is not found or unknown
    return "application/octet-stream";
}

int StaticRequestHandler::handle_request(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response) {

    response.version(request.version());
    response.result(boost::beast::http::status::ok);

    // Build file path from request target
    std::string file_path = base_dir_ + request.target().to_string();
    printf("file_path: %s\n", file_path.c_str());

    // Check if file exists
    std::ifstream file(file_path, std::ios::in | std::ios::binary);
    if (!file) {
        printf("File not found\n");
        response.result(boost::beast::http::status::not_found);
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = "File not found";
    } else {
        printf("File found\n");
        // Read file content into response body
        std::ostringstream content_stream;
        content_stream << file.rdbuf();
        response.body() = content_stream.str();
        response.content_length(response.body().size());

        // Set content type and status
        response.set(boost::beast::http::field::content_type, mime_type(file_path));
        response.result(boost::beast::http::status::ok);
    }

    // Prepare the response
    response.prepare_payload();

    return 0; // Return 0 to indicate success
    }
