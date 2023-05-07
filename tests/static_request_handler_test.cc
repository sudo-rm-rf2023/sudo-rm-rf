#include "static_request_handler.h"
#include "request_handler.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <string>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

// TODO: Use files dedicated to unittests
// TODO: Test getting files from sub-directories
// TODO: Test more complicated request_path
// TODO: Test the handler's response to non-GET requests
class StaticRequestHandlerTest : public ::testing::Test {
protected:
    StaticRequestHandlerTest() {}
    ~StaticRequestHandlerTest(){
        delete handler_;
    }
    StaticRequestHandler* handler_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

// Check that base_dir is required
TEST_F(StaticRequestHandlerTest, InvalidOptions){
    RequestHandler::Options invalid_options = {/*request_path=*/"/static",
                                                /*base_dir=*/std::nullopt};
    handler_ = StaticRequestHandler::makeStaticRequestHandler(invalid_options);
    EXPECT_EQ(handler_, nullptr);
}

TEST_F(StaticRequestHandlerTest, ExistingFile) {
    RequestHandler::Options options = {/*request_path=*/"/static",
                                        /*base_dir=*/"../static"};
    handler_ = StaticRequestHandler::makeStaticRequestHandler(options);
    ASSERT_NE(handler_, nullptr);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/static/index.html", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), 0);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/html");

    // TODO: Compare the response body with the expected content of the file
    // std::string expected_content = "your expected file content";
    // EXPECT_EQ(response_.body(), expected_content);
}

TEST_F(StaticRequestHandlerTest, ExistingFileWrongRequestPath) {
    RequestHandler::Options options = {/*request_path=*/"/static",
                                        /*base_dir=*/"../static"};
    handler_ = StaticRequestHandler::makeStaticRequestHandler(options);
    ASSERT_NE(handler_, nullptr);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/wrongpath/index.html", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), 0);
    EXPECT_EQ(response_.result_int(), 404);
}

TEST_F(StaticRequestHandlerTest, NonExistingFile) {
    RequestHandler::Options options = {/*request_path=*/"/data",
                                        /*base_dir=*/"../static"};
    handler_ = StaticRequestHandler::makeStaticRequestHandler(options);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/data/non_existing_file.html", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), 0);
    EXPECT_EQ(response_.result_int(), 404);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "File not found");
}

TEST_F(StaticRequestHandlerTest, FileOutsideBaseDir) {
    RequestHandler::Options options = {/*request_path=*/"/data",
                                        /*base_dir=*/"../static"};
    handler_ = StaticRequestHandler::makeStaticRequestHandler(options);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/data/../tests/static_file_outside_base.txt", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), 0);
    EXPECT_EQ(response_.result_int(), 403);
    EXPECT_EQ(response_[boost::beast::http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "Permission denied");
}

TEST_F(StaticRequestHandlerTest, ContentType) {
    RequestHandler::Options options = {/*request_path=*/"/www/images",
                                        /*base_dir=*/"../static"};
    handler_ = StaticRequestHandler::makeStaticRequestHandler(options);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/www/images/sudo-rm-rf-command.png", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), 0);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "image/png");

    // TODO: Compare the response body with the expected content of the file
    // std::string expected_content = "your expected file content";
    // EXPECT_EQ(response_.body(), expected_content);
}
