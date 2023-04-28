#include "static_request_handler.h"
#include "request_handler.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <string>

class StaticRequestHandlerTest : public ::testing::Test {
protected:
    StaticRequestHandlerTest() {
        boost::beast::http::request<boost::beast::http::string_body> request_{boost::beast::http::verb::get, "/", 11};
        request_.set(boost::beast::http::field::content_type, "text/plain");
        handler_ = new StaticRequestHandler("../static");
    }
    ~StaticRequestHandlerTest() {
        delete handler_;
    }
    StaticRequestHandler *handler_;
    boost::beast::http::request<boost::beast::http::string_body> request_;
    boost::beast::http::response<boost::beast::http::string_body> response_;
};

TEST_F(StaticRequestHandlerTest, ExistingFile) {
    request_.target("/index.html");
    ASSERT_EQ(handler_->handle_request(request_, response_), 0);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[boost::beast::http::field::content_type].to_string(), "text/html");

    // Optionally, compare the response body with the expected content of the file
    // std::string expected_content = "your expected file content";
    // EXPECT_EQ(response_.body(), expected_content);
}

TEST_F(StaticRequestHandlerTest, NonExistingFile) {
    request_.target("/non_existing_file.html");
    ASSERT_EQ(handler_->handle_request(request_, response_), 0);
    EXPECT_EQ(response_.result_int(), 404);
    EXPECT_EQ(response_[boost::beast::http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "File not found");
}

TEST_F(StaticRequestHandlerTest, ContentType) {
    request_.target("/sudo-rm-rf-command.png");
    ASSERT_EQ(handler_->handle_request(request_, response_), 0);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[boost::beast::http::field::content_type].to_string(), "image/png");

    // Optionally, compare the response body with the expected content of the file
    // std::string expected_content = "your expected file content";
    // EXPECT_EQ(response_.body(), expected_content);
}
