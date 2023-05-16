#include "utils.h"
#include "dispatcher.h"
#include "config_utils.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"

// TODO:
// Add error handling for undefined handler types
RequestHandlerFactory* create_handler_factory(DispatcherEntry entry){
    if (entry.handler_type == ECHO_HANDLER){
        return new EchoHandlerFactory(entry.location, entry.location_config);
    }
    if (entry.handler_type == STATIC_HANDLER){
        return new StaticHandlerFactory(entry.location, entry.location_config);
    }
    return nullptr;
}

// search if url is registered to a handler, else recersively search
// the next longest url
RequestHandlerFactory* Dispatcher::match(const std::string& request_url){
    auto it = routes_.find(request_url);
    if (it != routes_.end()) {
        return it->second;
    }

    // according to spec a 404 handler will always be configured to root url
    size_t last_slash_pos = request_url.find_last_of('/');
    last_slash_pos = (last_slash_pos==std::string::npos) ? 0 : last_slash_pos;
    std::string parentUrl = request_url.substr(0, last_slash_pos);
    if (parentUrl.length() == 0){
        parentUrl = "/";
    }
    return match(parentUrl);
}

status Dispatcher::assign_request(const http::request<http::string_body>& request,
    http::response<http::string_body>& response){

    std::string request_url = request.target().to_string();
    RequestHandlerFactory* factory = match(request_url);
    std::shared_ptr<RequestHandler> handler = factory->create();
    return handler->handle_request(request, response);
}

std::unordered_map<std::string, RequestHandlerFactory*> map_config_to_handler_factory(const NginxConfig &config){
    std::unordered_map<std::string, RequestHandlerFactory*> routes;
    std::optional<std::vector<DispatcherEntry>> dispatcher_entries = config_util::getDispatcherEntriesFromConfig(config);
    if (dispatcher_entries){
        for (DispatcherEntry entry : *dispatcher_entries){
            routes[entry.location] = create_handler_factory(entry);
        }
    }
    return routes;
}