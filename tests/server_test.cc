#include "config_manager.h"
#include "config_utils.h"
#include "server.h"
#include "session.h"
#include "gtest/gtest.h"
#include "router.h"

class serverTest : public ::testing::Test {
protected:
    serverTest() {}
    ~serverTest() {
        delete test_server;
    }
    boost::asio::io_service io_service;
    Dispatcher *dispatcher;
    std::unordered_map<std::string, RequestHandlerFactory*> routes;
    server *test_server;
};

TEST_F(serverTest, ConstructServer) {
    //TODO: make the port not hardcoded
    dispatcher = new Dispatcher(routes);
    test_server = new server(io_service, 80, dispatcher);
    using namespace std::chrono_literals;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::loopback(), 80);
    boost::asio::ip::tcp::socket socket(io_service);
    socket.connect(endpoint);
    io_service.run_for(1s);
    return;
}
// todo(yunqiu21): add tests for different cases
