#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "utils.h"
#include "request_handler.h"
#include "request_handler_factory.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/status.hpp>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class Dispatcher {
    public:
        Dispatcher(const std::unordered_map<std::string, RequestHandlerFactory*>& routes) {routes_ = routes;}
        status assign_request(
            const http::request<http::string_body> &request,
            http::response<http::string_body> &response) const;
        // generate 400 bad request
        void handle_bad_request(http::response<http::string_body>& response) const;

    private:
        std::unordered_map<std::string, RequestHandlerFactory*> routes_;
        RequestHandlerFactory* match(const std::string& request_url) const;
};

bool map_config_to_handler_factory(const NginxConfig &config, std::unordered_map<std::string, RequestHandlerFactory*> &routes);
RequestHandlerFactory* create_handler_factory(DispatcherEntry entry);

#endif