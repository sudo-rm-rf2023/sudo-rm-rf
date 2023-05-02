#include "router.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "logger.h"
#include <cstddef>
#include <memory>
#include <sched.h>
#include <sstream>
#include <iostream>

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
    std::string target = entry.request_target;
    std::string base_dir = entry.base_dir;
    std::unique_ptr<RequestHandler> handler(nullptr);

    switch(entry.handler_type){
        case ECHO_HANDLER:
            handler = std::make_unique<EchoRequestHandler>();
            break;
        case STATIC_HANDLER:
            handler = std::make_unique<StaticRequestHandler>(base_dir);
            break;
        default:
            break;
    }
    // register the hander to the url in the handler_table if the specified handler
    // type is valid, else skip the registration in question and log the issue
    if (handler){
        handler_table_[target] = std::move(handler);
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "invalid handler type received for target " << target;
        return 0;
    }
    return 1;
}


int Router::assign_request(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response){

    std::string target = request.target().to_string();
    int offset = 0;
    RequestHandler* handler = route_handler(target, offset);
    
    if (handler){
        // copy the request and remove the path to the handler in the request's path
        boost::beast::http::request<boost::beast::http::string_body> parsed_request = request;
        std::string new_target = target.substr(offset);
        parsed_request.target(new_target.empty() ? "/" : new_target); // set target to '/' if empty.
        handler->handle_request(parsed_request, response);
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "invalid request received for target " << target;
        handle_bad_request(response);
    }

    return 1;
}

std::string Router::mapping_to_string () {
    std::stringstream handler_stream;
    for (const auto& entry : handler_table_){
        const std::unique_ptr<RequestHandler>& handler = entry.second;
        // check the type of handler mapped to the endy
        if (handler->type() == ECHO_HANDLER) {
            handler_stream << entry.first<< " -> echo handler\n";
        }
        else if (handler->type() == STATIC_HANDLER) {
            handler_stream << entry.first<< " -> static handler\n";
        }
    }
    return handler_stream.str();
}

// takes in a string of request path and returns the ptr to the handler with longest 
// matching path. Returns nullptr if no matches are found
RequestHandler* Router::route_handler(std::string request_target, int& offset) {
    // if the request target is registered in handler_table_
    auto it = handler_table_.find(request_target);
    if (it != handler_table_.end()) {
        offset = request_target.length();
        return it->second.get();
    }

    // else search the parent directory
    size_t pos = request_target.find_last_of('/');
    if (request_target == "/" || pos == std::string::npos) {
        offset = 0;
        return nullptr;
    }

    std::string parentDir = request_target.substr(0, pos);
    if (parentDir.length() == 0){
        parentDir = "/";
    }
    return route_handler(parentDir, offset);
}

int Router::handle_bad_request(boost::beast::http::response<boost::beast::http::string_body>& response){
    response.result(boost::beast::http::status::bad_request);
    response.set(boost::beast::http::field::content_type, "text/plain");
    response.body() = "Invalid Request";
    response.prepare_payload();
    return 1;
}