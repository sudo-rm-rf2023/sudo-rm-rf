#include "block_request_handler.h"
#include "request_handler.h"
#include "gtest/gtest.h"
#include "config_parser.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class BlockRequestHandlerTest : public ::testing::Test {
protected:
    BlockRequestHandlerTest() {
        std::shared_ptr<NginxConfigStatement> config_statement(new NginxConfigStatement);
        config_statement->tokens_.push_back("sleep_time");
        config_statement->tokens_.push_back("1");
        NginxConfig config;
        config.statements_.push_back(std::move(config_statement));
        BlockRequestHandlerFactory factory("/block", config);
        handler_ = factory.create();

        NginxConfig empty_config;
        BlockRequestHandlerFactory default_factory("/default-block", empty_config);
        default_handler_ = default_factory.create();
    }

    std::shared_ptr<RequestHandler> handler_;
    std::shared_ptr<RequestHandler> default_handler_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

TEST_F(BlockRequestHandlerTest, BlockRequest){
    auto start = std::chrono::steady_clock::now();
    ASSERT_EQ(handler_->handle_request(request_, response_), true);
    auto end = std::chrono::steady_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    EXPECT_EQ(response_.result_int(), 200);
    EXPECT_EQ(response_[http::field::content_type].to_string(), "text/plain");
    EXPECT_EQ(response_.body(), "Blocking request complete");
    EXPECT_GE(elapsedSeconds, 1);
}
