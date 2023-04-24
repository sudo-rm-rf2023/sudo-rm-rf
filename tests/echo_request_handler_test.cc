#include "echo_request_handler.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <string>

// using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
// namespace http = boost::beast::http;    // from <boost/beast/http.hpp>


class EchoRequestHandlerTest : public ::testing::Test {
protected:
    EchoRequestHandlerTest() {
        boost::beast::http::request<boost::beast::http::string_body> request_{boost::beast::http::verb::post, "/", 11};
        request_.set(boost::beast::http::field::content_type, "text/plain");
    }
    ~EchoRequestHandlerTest() {
    }
    EchoRequestHandler handler_;
    boost::beast::http::request<boost::beast::http::string_body> request_;
    boost::beast::http::response<boost::beast::http::string_body> response_;
};

TEST_F(EchoRequestHandlerTest, HelloWorld){
    request_.body() = "Hello, world!"; 
    ASSERT_EQ(handler_.handle_request(request_, response_), 1);
    for (const auto &field : request_) {
        EXPECT_TRUE(response_.body().find(field.name_string().to_string()) != std::string::npos);
         EXPECT_TRUE(response_.body().find(field.value().to_string()) != std::string::npos);
    }
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[boost::beast::http::field::content_type].to_string(), "text/plain");
    EXPECT_TRUE(response_.body().find("Hello, world!") != std::string::npos);
}

TEST_F(EchoRequestHandlerTest, FakeDelimiter){
    std::string test_str = "Fake header delimiter 1 \r\n\r\n Fake header delimiter 2 \n\n Hello World";
    request_.body() = test_str; 
    ASSERT_EQ(handler_.handle_request(request_, response_), 1);
    EXPECT_TRUE(response_.body().find(test_str) != std::string::npos);
}