#include "config_manager.h"
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
    ConfigManager *config_manager;
    Router *router;

    server *test_server;
};

TEST_F(serverTest, ConstructServer) {
    config_manager = ConfigManager::makeConfigManager("server_test_config");
    router = Router::make_router(config_manager->getRouterEntries());
    test_server = new server(io_service, config_manager, router);
    using namespace std::chrono_literals;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::loopback(), config_manager->port());
    boost::asio::ip::tcp::socket socket(io_service);
    socket.connect(endpoint);
    io_service.run_for(1s);
    return;
}
// todo(yunqiu21): add tests for different cases
