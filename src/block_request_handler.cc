#include "block_request_handler.h"
#include "logger.h"
#include "config_utils.h"
#include <boost/beast.hpp>
#include <boost/beast/http/verb.hpp>
#include <string>
#include <unistd.h>

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

BlockRequestHandler::BlockRequestHandler(const std::string& location, const NginxConfig& config_block){
    std::optional<int> block_time = config_util::getBlockTimeFromLocationConfig(config_block);
    block_time_ = *block_time;
    BOOST_LOG_TRIVIAL(info) << "Block time for location " << location << " set to " << std::to_string(block_time_);
}

// sleep for configured seconds before sending back the response
status BlockRequestHandler::handle_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
    BOOST_LOG_TRIVIAL(info) << "BlockRequestHandler::handle_request called";
    BOOST_LOG_TRIVIAL(info) << "blocking request sleeping for " << std::to_string(block_time_) << " seconds...";
    sleep(block_time_);
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::content_type, "text/plain");
    response.body() = "Blocking request complete";
    response.prepare_payload();
    return true;
}