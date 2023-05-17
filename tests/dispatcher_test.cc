#include "dispatcher.h"
#include "config_parser.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"

class MockRequestHandlerFactory : public RequestHandlerFactory {
public:
    MockRequestHandlerFactory() : 
        RequestHandlerFactory("test", config_block) {}
    MOCK_METHOD0(create, std::shared_ptr<RequestHandler>());
private:
    NginxConfig config_block;
};

class TestHandler : public RequestHandler {
public:
    TestHandler(const std::string response_message):response_message_(response_message){}

    status handle_request(
        const http::request<http::string_body> &request,
        http::response<http::string_body> &response) override{
            response.version(request.version());
            response.result(http::status::ok);

            response.body() = response_message_;
            response.set(http::field::content_type, "text/plain");
            response.prepare_payload();
            return true;
        }
private:
    std::string response_message_;
};

class DispatcherTest : public ::testing::Test {
protected:
    DispatcherTest(): 
        dispatcher_(generate_routes()) {
    }
    std::unordered_map<std::string, RequestHandlerFactory*> generate_routes(){
        std::unordered_map<std::string, RequestHandlerFactory*> routes;
        routes["/echo"] = &echoFactory_;
        routes["/static"] = &staticFactory_;
        return routes;
    }
    
    MockRequestHandlerFactory echoFactory_;
    MockRequestHandlerFactory staticFactory_;
    NginxConfig placeholder_config_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    Dispatcher dispatcher_;
};

TEST_F(DispatcherTest, assignEcho){
    std::shared_ptr<RequestHandler> expectedHandler = std::make_shared<TestHandler>("echo");
    EXPECT_CALL(echoFactory_, create()).WillOnce(testing::Return(expectedHandler));
    request_.target("/echo");
    EXPECT_EQ(dispatcher_.assign_request(request_, response_), true);
    EXPECT_EQ(response_.body(), "echo");
}

TEST_F(DispatcherTest, assignStatic){
    std::shared_ptr<RequestHandler> expectedHandler = std::make_shared<TestHandler>("static");
    EXPECT_CALL(staticFactory_, create()).WillOnce(testing::Return(expectedHandler));
    request_.target("/static");
    EXPECT_EQ(dispatcher_.assign_request(request_, response_), true);
    EXPECT_EQ(response_.body(), "static");
}

TEST_F(DispatcherTest, NoURLMatch){
    request_.target("/random");
    EXPECT_FALSE(dispatcher_.assign_request(request_, response_));
}

TEST_F(DispatcherTest, NoURLMatch2){
    request_.target("/");
    EXPECT_FALSE(dispatcher_.assign_request(request_, response_));
}

// TODO:
// write more test cases such as for 404 handlers and nested request paths;
// write tests for helper functions in dispatcher