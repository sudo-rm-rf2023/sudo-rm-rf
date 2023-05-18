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
