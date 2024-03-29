#include "router.h"
#include "utils.h"
#include "request_handler.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "gtest/gtest.h"
#include <vector>
#include <string>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class RouterTest : public ::testing::Test {
protected:
    RouterTest() {
        entry1_.request_path = "/echo";
        entry1_.base_dir = "";
        entry1_.handler_type = ECHO_HANDLER;

        entry2_.request_path = "/static";
        entry2_.base_dir = "./test_static";
        entry2_.handler_type = STATIC_HANDLER;

        entry3_.request_path = "/";
        entry3_.base_dir = "";
        entry3_.handler_type = ECHO_HANDLER;

        router_entries_ = std::vector<RouterEntry> {entry1_, entry2_, entry3_};
        router_ = Router::make_router(router_entries_);
    }
    ~RouterTest() {
        delete router_;
    }
    RouterEntry entry1_;
    RouterEntry entry2_;
    RouterEntry entry3_;

    std::vector<RouterEntry> router_entries_;
    Router* router_;

    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

// TODO: Modify the tests to use mock RequestHandlers

// check if the router correctly assigns the http echo requests
TEST_F(RouterTest, AssignEcho) {
    request_.body() = "Hello, world!";
    request_.target("/echo");
    ASSERT_EQ(router_->assign_request(request_, response_), 1);
    for (const auto &field : request_) {
        EXPECT_TRUE(response_.body().find(field.name_string().to_string()) != std::string::npos);
        EXPECT_TRUE(response_.body().find(field.value().to_string()) != std::string::npos);
    }
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_TRUE(response_.body().find("Hello, world!") != std::string::npos);
}

// check if the router correctly assigns the http echo requests
TEST_F(RouterTest, AssignStatic) {
    request_.target("/static/index.html");
    ASSERT_EQ(router_->assign_request(request_, response_), 1);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/html");
}

// check if the router correctly assigns the handler for root "/"
TEST_F(RouterTest, AssignRoot) {
    request_.body() = "Hello, world!";
    request_.target("/");
    ASSERT_EQ(router_->assign_request(request_, response_), 1);
    for (const auto &field : request_) {
        EXPECT_TRUE(response_.body().find(field.name_string().to_string()) != std::string::npos);
        EXPECT_TRUE(response_.body().find(field.value().to_string()) != std::string::npos);
    }
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_TRUE(response_.body().find("Hello, world!") != std::string::npos);
}

// check if the router correctly assigns the handler for nested targets
TEST_F(RouterTest, AssignNested) {
    request_.body() = "Hello, world!";
    request_.target("/nested/path");
    ASSERT_EQ(router_->assign_request(request_, response_), 1);
    for (const auto &field : request_) {
        EXPECT_TRUE(response_.body().find(field.name_string().to_string()) != std::string::npos);
        EXPECT_TRUE(response_.body().find(field.value().to_string()) != std::string::npos);
    }
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_TRUE(response_.body().find("Hello, world!") != std::string::npos);
}

// check if the router correctly assigns the http echo requests
TEST_F(RouterTest, HandleBadRequest) {
    Router* router = Router::make_router(std::vector<RouterEntry> {entry1_});
    request_.body() = "Hello, world!";
    request_.target("/invalid/request/path");
    ASSERT_EQ(router->assign_request(request_, response_), 1);

    EXPECT_EQ(response_.result_int(), 400);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_TRUE(response_.body() == "Invalid Request");
}