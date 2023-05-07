#include "router.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "logger.h"
#include <cstddef>
#include <memory>
#include <sched.h>
#include <sstream>
#include <iostream>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

Router* Router::make_router(std::vector<RouterEntry> router_entries) {
    BOOST_LOG_TRIVIAL(info) << "Setting up router..";
    Router* router = new Router();
    for (RouterEntry entry : router_entries){
        if (!router->register_handler(entry)){
            delete router;
            return nullptr;
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Router setup complete";
    return router;
}

// return 1 if entry is successfully registered
int Router::register_handler(RouterEntry entry){
    RequestHandler::Options handler_options = {entry.request_path, entry.base_dir};
    RequestHandler* handler = nullptr;

    switch(entry.handler_type){
        case ECHO_HANDLER:
            handler = EchoRequestHandler::makeEchoRequestHandler(handler_options);
            break;
        case STATIC_HANDLER:
            handler = StaticRequestHandler::makeStaticRequestHandler(handler_options);
            break;
        default:
            break;
    }

    // register the hander to the url in the handler_table if the specified handler
    // type is valid, else skip the registration in question and log the issue
    if (handler){;
        handler_table_[entry.request_path] = std::unique_ptr<RequestHandler>(handler);
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "invalid handler type received for target " << entry.request_path;
        return 0;
    }
    return 1;
}


int Router::assign_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response){

    std::string target = request.target().to_string();
    RequestHandler* handler = route_handler(target);

    if (handler){
        handler->handle_request(request, response);
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "invalid request received for target " << target;
        handle_bad_request(response);
    }

    return 1;
}

// takes in a string of request path and returns the ptr to the handler with longest
// matching path. Returns nullptr if no matches are found
RequestHandler* Router::route_handler(std::string request_path) {
    // if the request target is registered in handler_table_
    auto it = handler_table_.find(request_path);
    if (it != handler_table_.end()) {
        return it->second.get();
    }

    // else search the parent directory
    size_t pos = request_path.find_last_of('/');
    if (request_path == "/" || pos == std::string::npos) {
        return nullptr;
    }

    std::string parentDir = request_path.substr(0, pos);
    if (parentDir.length() == 0){
        parentDir = "/";
    }
    return route_handler(parentDir);
}

int Router::handle_bad_request(http::response<http::string_body>& response){
    response.result(http::status::bad_request);
    response.set(http::field::content_type, "text/plain");
    response.body() = "Invalid Request";
    response.prepare_payload();
    return 1;
}