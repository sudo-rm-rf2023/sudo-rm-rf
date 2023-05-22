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

status CRUDApiHandler::handle_delete_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  std::string full_url_path = request.target().to_string();
  if (full_url_path.compare(0, request_path_.length(), request_path_) != 0) {
    BOOST_LOG_TRIVIAL(debug)
        << request_path_ << " is not a prefix of " << full_url_path
        << ". Cannot resolve to an aboslute path on filesystem.";
    response.result(http::status::bad_request);
    return false;
  }

  std::string file_path = full_url_path.substr(request_path_.length());
  std::string absolute_path = data_path_.value() + file_path;
  bool success = file_system_io_->delete_file(absolute_path);
  if (success) {
    response.result(http::status::no_content);
  } else {
    response.result(http::status::internal_server_error);
  }
  return success;
}
