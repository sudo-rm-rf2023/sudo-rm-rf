#include "echo_request_handler.h"
#include "request_handler.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <string>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class EchoRequestHandlerTest : public ::testing::Test {
protected:
    EchoRequestHandlerTest() {
        RequestHandler::Options options = {/*request_path=*/"/echoecho"};
        handler_ = EchoRequestHandler::makeEchoRequestHandler(options);
    }
    ~EchoRequestHandlerTest(){
        delete handler_;
    }
    EchoRequestHandler* handler_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

TEST_F(EchoRequestHandlerTest, HelloWorld){
    request_ = {boost::beast::http::verb::post,
                /*target=*/"/echoecho/123", /*version=*/11};
    request_.body() = "Hello, world!";
    request_.prepare_payload();
    ASSERT_EQ(request_.method_string(), std::string("POST"));
    ASSERT_EQ(request_.target(), std::string("/echoecho/123"));

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    for (const auto &field : request_) {
        EXPECT_TRUE(response_.body().find(field.name_string().to_string()) != std::string::npos);
        EXPECT_TRUE(response_.body().find(field.value().to_string()) != std::string::npos);
    }
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_TRUE(response_.body().find("Hello, world!") != std::string::npos);
    EXPECT_TRUE(response_.body().find("POST /echoecho/123 HTTP/1.1") != std::string::npos);
}

TEST_F(EchoRequestHandlerTest, FakeDelimiter){
    std::string test_str = "Fake header delimiter 1 \r\n\r\n Fake header delimiter 2 \n\n Hello World";
    request_ = {boost::beast::http::verb::post,
                /*target=*/"/echoecho/123", /*version=*/11};
    request_.body() = test_str;
    request_.prepare_payload();

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_TRUE(response_.body().find(test_str) != std::string::npos);
}

TEST_F(EchoRequestHandlerTest, BadRequest){
    ASSERT_EQ(handler_->handle_bad_request(response_), false);
    EXPECT_EQ(response_.result_int(), 400);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_TRUE(response_.body() == "Invalid Request");
}