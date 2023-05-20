#ifndef CRUD_API_HANDLER_H
#define CRUD_API_HANDLER_H

#include "config_parser.h"
#include "logger.h"
#include "request_handler.h"
#include "request_handler_factory.h"

namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

class CRUDApiHandler : public RequestHandler {
 public:
  CRUDApiHandler(const std::string &location, const NginxConfig &config_block);

  status handle_request(const http::request<http::string_body> &request,
                        http::response<http::string_body> &response) override;

 private:
  /*
   * API: Creates an entity and responds with the new ID.
   * Internally: Finds next available ID and writes req body to
   * {data_path}/{entity}/{ID}. Returns {"id": {ID}} in the response.
   */
  status handle_create_request(const http::request<http::string_body> &request,
                               http::response<http::string_body> &response);

  /*
   * API: Returns the data for previously created entity + ID.
   * Internally: Reads the stored {data_path}/{entity}/{ID} and resp to user.
   */
  status handle_retrieve_request(
      const http::request<http::string_body> &request,
      http::response<http::string_body> &response);

  /*
   * API: Updates the data for entity/ID with new data, resp w/ status.
   * Internally: writes req body to
   * {data_path}/{entity}/{ID}, responds "SUCCESS" or "FAIL" if DNE.
   */
  status handle_update_request(const http::request<http::string_body> &request,
                               http::response<http::string_body> &response);
  /*
   * API: Delete entity w/ ID from the system
   * Internally: Removes file {data_path}/{entity}/{ID} and respond "SUCCESS" if
   * it exists, else response "FAIL"
   */
  status handle_delete_request(const http::request<http::string_body> &request,
                               http::response<http::string_body> &response);
  /*
   * API: Returns a JSON list of valid IDs for the given Entity.
   * Internally: Lists filenames in {data_path}/{entity}. Returns the list of file
   * names in the response, or empty list for no existing entity.
   */
  status handle_list_request(const http::request<http::string_body> &request,
                             http::response<http::string_body> &response);
};

class CRUDApiHandlerFactory : public RequestHandlerFactory {
 public:
  CRUDApiHandlerFactory(const std::string &location,
                        const NginxConfig &config_block)
      : RequestHandlerFactory(location, config_block) {}

  std::shared_ptr<RequestHandler> create() {
    return std::make_shared<CRUDApiHandler>(location_, config_block_);
  }
};

#endif
