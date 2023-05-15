#include "static_request_handler.h"
#include "config_parser.h"
#include "config_utils.h"
#include "logger.h"
#include "utils.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace fs = boost::filesystem;

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

// Constructor
StaticRequestHandler::StaticRequestHandler(const std::string &location, const NginxConfig &config_block) {
    request_path_ = location;
    base_dir_ = config_util::getBaseDirFromLocationConfig(config_block);
}

StaticRequestHandler::StaticRequestHandler(const RequestHandler::Options &options) {
    request_path_ = options.request_path;
    base_dir_ = options.base_dir.value_or(std::string());
    BOOST_LOG_TRIVIAL(info) << "StaticRequestHandler created. Base directory:" << *base_dir_;
}

// Factory Method
StaticRequestHandler *StaticRequestHandler::makeStaticRequestHandler(const RequestHandler::Options &options) {
    if (options.base_dir.has_value()) {
        return new StaticRequestHandler(options);
    } else { // invalid options: StaticRequestHandler requires a base_dir.
        return nullptr;
    }
}

status StaticRequestHandler::handle_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {

    response.version(request.version());
    response.result(http::status::ok);

    // Build file path from request target
    int offset = request_path_.length();
    std::string requested_file = request.target().to_string().substr(offset);
    std::string file_path = *base_dir_ + requested_file;
    BOOST_LOG_TRIVIAL(info) << "StaticRequestHandler::handle_request called";
    BOOST_LOG_TRIVIAL(info) << "Request method: " << request.method_string();
    BOOST_LOG_TRIVIAL(info) << "file_path: " << file_path;

    if (!fs::exists(file_path) || !fs::is_regular_file(file_path)) {
        BOOST_LOG_TRIVIAL(info) << "File not found";
        response.result(http::status::not_found);
        response.set(http::field::content_type, "text/plain");
        response.body() = "File not found";
    } else {
        // Security check - verify whether the file path is under the base directory
        std::string base_canonical = fs::canonical(*base_dir_).string();
        std::string file_canonical = fs::canonical(file_path).string();

        // Check if base_canonical is a prefix of file_canonical
        if (file_canonical.find(base_canonical) != 0) {
            BOOST_LOG_TRIVIAL(info) << "Permission denied";
            response.result(http::status::forbidden);
            response.set(http::field::content_type, "text/plain");
            response.body() = "Permission denied";
        } else {
            // File exists under base dir
            std::ifstream file(file_path, std::ios::in | std::ios::binary);

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
    }

    // Prepare the response
    response.prepare_payload();

    return true; // Return 0 to indicate success
}
