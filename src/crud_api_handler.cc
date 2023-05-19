#include "crud_api_handler.h"

#include "config_utils.h"

CRUDApiHandler::CRUDApiHandler(
    const std::string &location, const NginxConfig &config_block,
    std::shared_ptr<FileSystemIOInterface> file_system_io) {
  request_path_ = location;
  data_path_ = config_util::getDataPathFromLocationConfig(config_block);
  file_system_io_ = file_system_io;
}

status CRUDApiHandler::handle_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "Handling request to CRUDApiHandler...";

  // TODO add implementation details
  return false;
}
