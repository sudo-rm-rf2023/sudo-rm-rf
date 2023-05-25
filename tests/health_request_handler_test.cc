#include "health_request_handler.h"
#include "request_handler.h"
#include "gtest/gtest.h"
#include "config_parser.h"
#include <fstream>
#include <iostream>
#include <string>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class HealthRequestHandlerTest : public ::testing::Test {
protected:
    HealthRequestHandlerTest() {
        NginxConfig config;
        HealthRequestHandlerFactory factory("/health", config);
        handler_ = factory.create();
    }

    std::shared_ptr<RequestHandler> handler_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

TEST_F(HealthRequestHandlerTest, ReturnOK){
    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "OK");
}