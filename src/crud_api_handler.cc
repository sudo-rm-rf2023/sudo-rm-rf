#include "crud_api_handler.h"

#include "config_utils.h"
#include <optional>

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

std::string CRUDApiHandler::create_absolute_file_path(const http::request<http::string_body> &request) {
  std::string full_url_path = request.target().to_string();
  if (full_url_path.compare(0, request_path_.length(), request_path_) != 0) {
    return ""; // cannot find valid path
  }

  std::string file_path = full_url_path.substr(request_path_.length());
  return data_path_.value() + file_path;
}

status CRUDApiHandler::handle_delete_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "Handling request for deleting...";

  std::string absolute_path = create_absolute_file_path(request);
  if (absolute_path == "") {
    BOOST_LOG_TRIVIAL(debug)
    << request_path_ << " is not a prefix of " << request.target().to_string()
    << ". Cannot resolve to an absolute path on filesystem.";
    response.result(http::status::bad_request);
    return false;
  }

  bool success = file_system_io_->delete_file(absolute_path);
  if (success) {
    response.result(http::status::no_content);
  } else {
    response.result(http::status::internal_server_error);
  }
  return success;
}

status CRUDApiHandler::handle_retrieve_request(const http::request<http::string_body> &request, http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "Handling request for deleting...";

  std::string absolute_path = create_absolute_file_path(request);
  if (absolute_path == "") {
    BOOST_LOG_TRIVIAL(debug)
    << request_path_ << " is not a prefix of " << request.target().to_string()
    << ". Cannot resolve to an absolute path on filesystem.";
    response.result(http::status::bad_request);
    return false;
  }

  std::optional<std::ostringstream> object = file_system_io_->read_file(absolute_path);
  if (object.has_value()) {
    response.result(http::status::ok);
    response.body() = object.value().str();
    BOOST_LOG_TRIVIAL(trace) << "Retrieved object.";
    return true;
  } else {
    response.result(http::status::not_found);
    BOOST_LOG_TRIVIAL(trace) << "Cannot retrieve object; not found.";
    return false;
  }
}

status CRUDApiHandler::handle_list_request(const http::request<http::string_body> &request, http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "Handling request for LIST...";

  std::string absolute_path = create_absolute_file_path(request);
  if (absolute_path == "") {
    BOOST_LOG_TRIVIAL(debug)
    << request_path_ << " is not a prefix of " << request.target().to_string()
    << ". Cannot resolve to an absolute path on filesystem.";
    response.result(http::status::bad_request);
    return false;
  }
  
  std::optional<std::vector<std::string>> files = file_system_io_->ls(absolute_path);
  if (files.has_value()) {
    // Convert list of strings into a string with comma separated values
    const std::vector<std::string>& data = files.value();
    std::string responseBody = "[";
    for (size_t i = 0; i < data.size(); ++i)
    {
        responseBody += data[i];
        if (i != data.size() - 1)
        {
            responseBody += ",";
        }
    }
    responseBody += "]";
    response.result(http::status::ok);
    response.body() = responseBody;
    BOOST_LOG_TRIVIAL(trace) << "Body propagated with list of Entities.";
    return true;
  } else {
    response.result(http::status::not_found);
    BOOST_LOG_TRIVIAL(trace) << "Cannot retrieve directory.";
    return false;
  }
}
