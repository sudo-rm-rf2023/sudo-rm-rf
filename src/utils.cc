#include "utils.h"

// A helper function that returns the corresponding mime_type given the request_path
std::string mime_type(const std::string& path) {
    // Mapping of file extensions to MIME types
    static const std::unordered_map<std::string, std::string> mime_types{
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".json", "application/json"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
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

namespace util {

std::vector<std::string> parseURL(const std::string &url)
{
    std::vector<std::string> parsed_url_tokens;
    std::size_t slash_pos = url.find("/");
    while (slash_pos != std::string::npos){
        std::size_t next_slash_pos = url.find("/", slash_pos + 1);
        std::string new_token;
        if (next_slash_pos == std::string::npos){
            new_token = url.substr(slash_pos + 1);
        } else {
            size_t token_length = next_slash_pos - slash_pos - 1;
            new_token = url.substr(slash_pos + 1, token_length);
        }
        if (!new_token.empty()){
            parsed_url_tokens.push_back(new_token);
        }
        slash_pos = next_slash_pos;
    }
    return parsed_url_tokens;
}

} // namespace util
