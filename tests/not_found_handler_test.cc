#include "not_found_handler.h"
#include "request_handler.h"
#include "gtest/gtest.h"
#include "config_parser.h"
#include <fstream>
#include <iostream>
#include <string>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class NotFoundHandlerTest : public ::testing::Test {
protected:
    NotFoundHandlerTest() {
        NginxConfig config;
        NotFoundHandlerFactory factory("/", config);
        handler_ = factory.create();
    }

    std::shared_ptr<RequestHandler> handler_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

TEST_F(NotFoundHandlerTest, BadLocation){
    request_ = {boost::beast::http::verb::post,
                /*target=*/"/bad/location", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.version(), request_.version());
    EXPECT_EQ(response_.result_int(), 404);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "Not Found");
}

TEST_F(NotFoundHandlerTest, EmptyLocation){
    request_ = {boost::beast::http::verb::post,
                /*target=*/"", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.version(), request_.version());
    EXPECT_EQ(response_.result_int(), 404);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "Not Found");
}
