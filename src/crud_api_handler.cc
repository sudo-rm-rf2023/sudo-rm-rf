#include "crud_api_handler.h"
#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>

#include <optional>
#include <vector>
#include <algorithm>

#include "config_utils.h"

namespace http = boost::beast::http;

CRUDApiHandler::CRUDApiHandler(
    const std::string &location, const NginxConfig &config_block,
    std::shared_ptr<FileSystemIOInterface> file_system_io) {
  BOOST_LOG_TRIVIAL(trace) << "Creating CRUDApiHandler...";
  request_path_ = location;
  data_path_ = config_util::getDataPathFromLocationConfig(config_block);
  file_system_io_ = file_system_io;
}

/**
 * Given a request object, generate a response for the appropriate CRUD operation on Entities.
*/
status CRUDApiHandler::handle_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  http::verb req_method = request.method();
  BOOST_LOG_TRIVIAL(trace) << "Handling request to CRUDApiHandler. Method="
                           << req_method;

  response.clear();
  response.version(11);
  response.set(http::field::content_type, "text/plain");

  bool status_;
  switch (req_method) {
    case http::verb::get:
      if (isFile(request.target().to_string())) {
        BOOST_LOG_TRIVIAL(trace) << "Processing RETRIEVE request...";
        status_ = handle_retrieve_request(request, response);
        break;
      }
      else if (isDirectory(request.target().to_string())) {
        BOOST_LOG_TRIVIAL(trace) << "Processing LIST request...";
        status_ = handle_list_request(request, response);
        break;
      }
      else {
        BOOST_LOG_TRIVIAL(trace) << "GET request cannot be processed - invalid requested path.\n" << "Allowed requested path formats: '/api/Shoes' or '/api/Shoes/1'";
        response.result(http::status::not_found);
        response.body() = "Unsupported HTTP verb provided: " +
                    http::to_string(req_method).to_string();
        status_ = false;
        break;
      }
    case http::verb::post:
      BOOST_LOG_TRIVIAL(trace) << "Processing POST/CREATE request...";
      status_ = handle_create_request(request, response);
      break;
    case http::verb::put:
      BOOST_LOG_TRIVIAL(trace) << "Processing PUT/UPDATE request...";
      status_ = handle_update_request(request, response);
      break;
    case http::verb::delete_:
      BOOST_LOG_TRIVIAL(trace) << "Processing DELETE request...";
      status_ = handle_delete_request(request, response);
      break;
    default:
      BOOST_LOG_TRIVIAL(trace)
          << "Processing unknown request: " << http::to_string(req_method);
      response.result(http::status::internal_server_error);
      response.body() = "Unsupported HTTP verb provided: " +
                   http::to_string(req_method).to_string();
      status_ = false;
  }

  response.prepare_payload();
  return status_;
}

std::string CRUDApiHandler::create_absolute_file_path(
    const http::request<http::string_body> &request) {
  std::string full_url_path = request.target().to_string();
  if (full_url_path.compare(0, request_path_.length(), request_path_) != 0) {
    return "";  // cannot find valid path
  }

  std::string file_path = full_url_path.substr(request_path_.length());
  return data_path_.value() + file_path;
}

status CRUDApiHandler::handle_create_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "handling create request";

  int max_id = 0;  // IDs start at 1 for each entity, 0 to account for increment
  std::string absolute_dir_path = create_absolute_file_path(request);
  if (absolute_dir_path == "") {
    BOOST_LOG_TRIVIAL(error)
        << "Failed to generate absolute path with target :" << request.target();
    response.result(http::status::not_found);
    return false;
  }
  BOOST_LOG_TRIVIAL(debug) << "absolute path: " << absolute_dir_path;
  const std::optional<std::vector<std::string>> &entities =
      file_system_io_->ls(absolute_dir_path);

  if (entities.has_value()) {
    BOOST_LOG_TRIVIAL(trace) << "Finding ID for entry";
    // since the file names are just the ID, loop to find largest id
    for (const std::string &ent : entities.value()) {
      try {
        int id = std::stoi(ent);
        if (id > max_id) {
          max_id = id;
        }
      } catch (...) {
        BOOST_LOG_TRIVIAL(error) << "Directory entry is not `int ID`: " << ent;
        continue;
      }
    }
  } else {
    BOOST_LOG_TRIVIAL(debug) << "No existing directory at requested path: \""
                             << absolute_dir_path << "\"";
  }
  max_id++;

  std::string entity_file_path =
      absolute_dir_path + "/" + std::to_string(max_id);  // update the abs path
  BOOST_LOG_TRIVIAL(info) << "Creating entity at requested path: \""
                          << entity_file_path << "\"";
  std::ostringstream contents;
  contents << request.body();
  BOOST_LOG_TRIVIAL(info) << "Writing request body to new entity";
  bool success = file_system_io_->write_file(entity_file_path, contents);
  if (!success) {
    BOOST_LOG_TRIVIAL(error) << "Failed to create file: " << entity_file_path;
    response.result(http::status::internal_server_error);
    return false;
  }
  // Set content type and status
  std::string json_label = "id";
  std::string json_value = std::to_string(max_id);
  std::string body_str = formatJsonObject(json_label,json_value );
  response.result(http::status::ok);
  response.set(http::field::content_type, mime_type(".json"));
  response.body() = body_str;
  return true;
}
/**
 * @brief API: Updates the data for entity/ID with new data, resp w/ status.
 * Internally: writes req body to
 * {data_path}/{entity}/{ID}, responds "SUCCESS" or "FAIL" if DNE.
 * @param request
 * @param response
 * @return status
 */
status CRUDApiHandler::handle_update_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "handling update request";
  std::string absolute_path = create_absolute_file_path(request);
  BOOST_LOG_TRIVIAL(trace) << "Handling request for retrieve";
  if (absolute_path == "") {
    BOOST_LOG_TRIVIAL(debug)
        << request_path_ << " is not a prefix of "
        << request.target().to_string()
        << ". Cannot resolve to an absolute path on filesystem.";
    response.result(http::status::not_found);
    return false;
  }
  // check if file exists
  std::optional<std::ostringstream> object =
      file_system_io_->read_file(absolute_path);
  if (object.has_value()) {
    response.result(http::status::ok);
    BOOST_LOG_TRIVIAL(trace) << "found object.";
  } else {
    response.result(http::status::not_found);
    BOOST_LOG_TRIVIAL(trace) << "Cannot update object: not found.";
    return false;
  }
  // since file exists, update it
  std::ostringstream content;
  content << request.body();
  bool success = file_system_io_->write_file(absolute_path, content);
  if (success) {
    BOOST_LOG_TRIVIAL(trace) << "wrote content to entity: " << request.target();
    response.result(http::status::ok);
  } else {
    BOOST_LOG_TRIVIAL(trace)
        << "failed to write content to entity: " << request.target();
    response.result(http::status::internal_server_error);
  }
  response.prepare_payload();
  return success;
}

/**
 * @brief API: Delete entity w/ ID from the system
 * Internally: Removes file {data_path}/{entity}/{ID} and respond "SUCCESS" ifit
 * exists, else response "FAIL"
 * @param request
 * @param response
 * @return status
 */
status CRUDApiHandler::handle_delete_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "Handling request for deleting...";

  std::string absolute_path = create_absolute_file_path(request);
  if (absolute_path == "") {
    BOOST_LOG_TRIVIAL(debug)
        << request_path_ << " is not a prefix of "
        << request.target().to_string()
        << ". Cannot resolve to an absolute path on filesystem.";
    response.result(http::status::not_found);
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

status CRUDApiHandler::handle_retrieve_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "Handling request for retrieve";

  std::string absolute_path = create_absolute_file_path(request);
  if (absolute_path == "") {
    BOOST_LOG_TRIVIAL(debug)
        << request_path_ << " is not a prefix of "
        << request.target().to_string()
        << ". Cannot resolve to an absolute path on filesystem.";
    response.result(http::status::not_found);
    return false;
  }

  std::optional<std::ostringstream> object =
      file_system_io_->read_file(absolute_path);
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

status CRUDApiHandler::handle_list_request(
    const http::request<http::string_body> &request,
    http::response<http::string_body> &response) {
  BOOST_LOG_TRIVIAL(trace) << "Handling request for LIST...";

  std::string absolute_path = create_absolute_file_path(request);
  if (absolute_path == "") {
    BOOST_LOG_TRIVIAL(debug)
        << request_path_ << " is not a prefix of "
        << request.target().to_string()
        << ". Cannot resolve to an absolute path on filesystem.";
    response.result(http::status::not_found);
    return false;
  }

  std::optional<std::vector<std::string>> files =
      file_system_io_->ls(absolute_path);
  if (files.has_value()) {
    // Convert list of strings into a string with comma separated values
    std::vector<std::string> &data = files.value();
    std::sort(data.begin(), data.end());
    std::string responseBody = "[";
    for (size_t i = 0; i < data.size(); ++i) {
      responseBody += stripLeadingSlash(data[i]);
      if (i != data.size() - 1) {
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

std::string CRUDApiHandler::formatJsonObject(const std::string &label, const std::string &value){
  std::string ret;
  ret = "{\"" + label + "\":" + value + "}";
  return ret;
}

bool CRUDApiHandler::isFile(const std::string& target)
{
    size_t slashCount = std::count(target.begin(), target.end(), '/');
    return slashCount == 3;  // eg: "/api/Shoes/1"
}

bool CRUDApiHandler::isDirectory(const std::string& target)
{
    size_t slashCount = std::count(target.begin(), target.end(), '/');
    return slashCount == 2;  // eg: "/api/Shoes"
}

std::string CRUDApiHandler::stripLeadingSlash(const std::string& str)
{
    if (!str.empty() && str[0] == '/')
    {
        return str.substr(1);
    }
    else
    {
        return str;
    }
}
