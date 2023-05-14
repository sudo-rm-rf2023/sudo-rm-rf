#include "dispatcher.h"
#include "config_parser.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"

class MockEchoHandlerFactory : public EchoHandlerFactory {
public: 
    MOCK_METHOD0(create, std::shared_ptr<RequestHandler>());
}

class MockStaticHandlerFactory : public StaticHandlerFactory {
public: 
    MOCK_METHOD0(create, std::shared_ptr<RequestHandler>());
}

class DispatcherTest : public ::testing::Test {
protected:
    DispatcherTest() {
        // TODO:
        // create the nginx config
        // create handler factorys
        // init routes
    }
    ~DispatcherTest() {
        delete staticFactory_;
        delete echoFactory_;
    }
    std::unordered_map<std::string, RequestHandlerFactory*> routes_;
    MockStaticHandlerFactory* staticFactory_;
    MockEchoHandlerFactory* echoFactory_;
    NginxConfig static_config_;
    NginxConfig echo_config_;
};

