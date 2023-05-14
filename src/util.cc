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

