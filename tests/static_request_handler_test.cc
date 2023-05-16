#include "static_request_handler.h"
#include "request_handler.h"
#include "gtest/gtest.h"
#include "config_parser.h"
#include <fstream>
#include <iostream>
#include <string>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

NginxConfig write_static_config(std::string base_dir) {
  NginxConfig config;

  NginxConfigStatement* statement = new NginxConfigStatement();
  statement->tokens_.push_back("root");
  statement->tokens_.push_back(base_dir);

  config.statements_.emplace_back(std::shared_ptr<NginxConfigStatement>(statement));
  return config;
}

// TODO: Test more complicated request_path
// TODO: Test the handler's response to non-GET requests
class StaticRequestHandlerTest : public ::testing::Test {
protected:
    StaticRequestHandlerTest() {}
    ~StaticRequestHandlerTest() {
        delete factory_;
    }
    NginxConfig config_;
    StaticHandlerFactory *factory_;
    std::shared_ptr<RequestHandler> handler_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

TEST_F(StaticRequestHandlerTest, ExistingFile) {
    config_ = write_static_config("../tests/test_basedir");
    factory_ = new StaticHandlerFactory("/static", config_);
    handler_ = factory_->create();
    ASSERT_NE(handler_, nullptr);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/static/index.html", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/html");

    // TODO: Compare the response body with the expected content of the file
    // std::string expected_content = "your expected file content";
    // EXPECT_EQ(response_.body(), expected_content);
}

TEST_F(StaticRequestHandlerTest, ExistingFileWrongRequestPath) {
    config_ = write_static_config("../tests/test_basedir");
    factory_ = new StaticHandlerFactory("/static", config_);
    handler_ = factory_->create();
    ASSERT_NE(handler_, nullptr);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/wrongpath/index.html", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.result_int(), 404);
}

TEST_F(StaticRequestHandlerTest, NonExistingFile) {
    config_ = write_static_config("../tests/test_basedir");
    factory_ = new StaticHandlerFactory("/data", config_);
    handler_ = factory_->create();
    ASSERT_NE(handler_, nullptr);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/data/non_existing_file.html", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.result_int(), 404);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "File not found");
}

TEST_F(StaticRequestHandlerTest, FileOutsideBaseDir) {
    config_ = write_static_config("../tests/test_basedir/nested");
    factory_ = new StaticHandlerFactory("/data", config_);
    handler_ = factory_->create();
    ASSERT_NE(handler_, nullptr);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/data/../static_file_outside_base.txt", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.result_int(), 403);
    EXPECT_EQ(response_[boost::beast::http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "Permission denied");
}

TEST_F(StaticRequestHandlerTest, NestedFile) {
    config_ = write_static_config("../tests/test_basedir");
    factory_ = new StaticHandlerFactory("/data", config_);
    handler_ = factory_->create();
    ASSERT_NE(handler_, nullptr);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/data/nested/sudo-rm-rf.txt", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[boost::beast::http::field::content_type].to_string(), "text/plain");
}

TEST_F(StaticRequestHandlerTest, ContentType) {
    config_ = write_static_config("../tests/test_basedir");
    factory_ = new StaticHandlerFactory("/www/images", config_);
    handler_ = factory_->create();
    ASSERT_NE(handler_, nullptr);

    request_ = {boost::beast::http::verb::get,
                /*target=*/"/www/images/sudo-rm-rf-command.png", /*version=*/11};

    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "image/png");

    // TODO: Compare the response body with the expected content of the file
    // std::string expected_content = "your expected file content";
    // EXPECT_EQ(response_.body(), expected_content);
}
