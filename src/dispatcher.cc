#include "utils.h"
#include "dispatcher.h"
#include "config_utils.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "not_found_handler.h"
#include "crud_api_handler.h"
#include "health_request_handler.h"
#include "block_request_handler.h"
#include "score_request_handler.h"
#include "logger.h"

// TODO:
// Add error handling for undefined handler types
RequestHandlerFactory* create_handler_factory(DispatcherEntry entry){
    if (entry.handler_type == ECHO_HANDLER){
        return new EchoHandlerFactory(entry.location, entry.location_config);
    }
    if (entry.handler_type == STATIC_HANDLER){
        return new StaticHandlerFactory(entry.location, entry.location_config);
    }
    if (entry.handler_type == NOTFOUND_HANDLER){
        return new NotFoundHandlerFactory(entry.location, entry.location_config);
    }
    if (entry.handler_type == CRUD_API_HANDLER){
        return new CRUDApiHandlerFactory(entry.location, entry.location_config);
    }
    if (entry.handler_type == HEALTH_REQUEST_HANDLER){
        return new HealthRequestHandlerFactory(entry.location, entry.location_config);
    }
    if (entry.handler_type == BLOCK_REQUEST_HANDLER){
        return new BlockRequestHandlerFactory(entry.location, entry.location_config);
    }
    if (entry.handler_type == SCORE_REQUEST_HANDLER){
        return new ScoreRequestHandlerFactory(entry.location, entry.location_config);
    }
    return nullptr;
}

// search if url is registered to a handler, else recersively search
// the next longest url
RequestHandlerFactory* Dispatcher::match(const std::string& request_url) const{
    auto it = routes_.find(request_url);
    if (it != routes_.end()) {
        return it->second;
    }

    if(request_url == "/"){
        return nullptr;
    }

    size_t last_slash_pos = request_url.find_last_of('/');
    last_slash_pos = (last_slash_pos==std::string::npos) ? 0 : last_slash_pos;
    std::string parentUrl = request_url.substr(0, last_slash_pos);
    if (parentUrl.length() == 0){
        parentUrl = "/";
    }
    return match(parentUrl);
}

status Dispatcher::assign_request(const http::request<http::string_body>& request,
    http::response<http::string_body>& response) const{

    std::string request_url = request.target().to_string();

    BOOST_LOG_TRIVIAL(info) << REQUEST_PATH << request_url;
    BOOST_LOG_TRIVIAL(info) << REQUEST_METHOD << request.method_string();

    RequestHandlerFactory* factory = match(request_url);
    if(factory == nullptr){
        return false;
    }
    std::shared_ptr<RequestHandler> handler = factory->create();
    return handler->handle_request(request, response);
}

 bool map_config_to_handler_factory(const NginxConfig &config, std::unordered_map<std::string, RequestHandlerFactory*> &routes){
    std::optional<std::vector<DispatcherEntry>> dispatcher_entries = config_util::getDispatcherEntriesFromConfig(config);
    if (dispatcher_entries){
        for (DispatcherEntry entry : *dispatcher_entries){
            RequestHandlerFactory*  factory = create_handler_factory(entry);
            if (factory){
                routes[entry.location] =factory;
            }
            else {
                BOOST_LOG_TRIVIAL(error) << "invalid handler config for location" << entry.location;
                return false;
            }
        }
    }
    return true;
}

void Dispatcher::handle_bad_request(http::response<http::string_body>& response) const{
    response.result(http::status::bad_request);
    response.set(http::field::content_type, "text/plain");
    response.body() = "Invalid Request";
    response.prepare_payload();
}
