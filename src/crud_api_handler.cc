#include "crud_api_handler.h"

CRUDApiHandler::CRUDApiHandler(const std::string &location,
                               const NginxConfig &config_block) {
  // TODO add implemetation details
}

status CRUDApiHandler::handle_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "Handling request to CRUDApiHandler...";

  // TODO add implementation details
  return false;
}
