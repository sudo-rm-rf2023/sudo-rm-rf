#ifndef ROUTER_H
#define ROUTER_H

#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/status.hpp>

#include "utils.h"
#include "request_handler.h"

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class Router {
    public:
        Router() {}
        static Router* make_router(std::vector<RouterEntry> router_entries);

        int assign_request(
        const http::request<http::string_body>& request,
        http::response<http::string_body>& response);

        int handle_bad_request(http::response<http::string_body>& response);
    private:
        int register_handler(RouterEntry entry);
        std::unordered_map<std::string, std::unique_ptr<RequestHandler>> handler_table_;

        // returns a pointer to the handler to be invoked and set path_idx to the
        // index of the '/' after the path to handler
        RequestHandler* route_handler(std::string request_target);
};

#endif