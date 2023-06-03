#ifndef SCORE_REQUEST_HANDLER_H
#define SCORE_REQUEST_HANDLER_H

#include "logger.h"
#include "boost_file_system_io.h"
#include "request_handler.h"
#include "request_handler_factory.h"
#include "default_score_manager.h"

namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

class ScoreRequestHandler : public RequestHandler{
public:
    ScoreRequestHandler(const std::string &location, const NginxConfig &config_block,
                        std::shared_ptr<ScoreManager> score_manager)
                            : score_manager_(score_manager), request_path_(location){}

    status handle_request(const http::request<http::string_body> &request,
                        http::response<http::string_body> &response) override;
private:
    std::shared_ptr<ScoreManager> score_manager_;
    const std::string request_path_;
};

class ScoreRequestHandlerFactory : public RequestHandlerFactory {
public:
    ScoreRequestHandlerFactory(const std::string &location,
                                const NginxConfig &config_block);
    std::shared_ptr<RequestHandler> create();

private:
    std::shared_ptr<ScoreManager> score_manager_; // long-lived score_manager
};


#endif