#include "crud_api_handler.h"

#include "gtest/gtest.h"
#include "request_handler.h"

namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

class CRUDApiHandlerTest : public ::testing::Test {
 protected:
  CRUDApiHandlerTest() {
    NginxConfig config;
    CRUDApiHandlerFactory factory("/", config);
    handler_ = factory.create();
  }

  std::shared_ptr<RequestHandler> handler_;
  http::request<http::string_body> request_;
  http::response<http::string_body> response_;
};

TEST_F(CRUDApiHandlerTest, SmokeTest) {
    bool success = handler_->handle_request(request_, response_);
    ASSERT_FALSE(success);
}
