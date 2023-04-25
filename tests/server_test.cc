#include "config_manager.h"
#include "server.h"
#include "session.h"
#include "gtest/gtest.h"

class serverTest : public ::testing::Test {
protected:
    serverTest() {}
    ~serverTest() {
        delete config_manager;
        delete test_server;
    }
    boost::asio::io_service io_service;
    ConfigManager *config_manager;
    server *test_server;
};

TEST_F(serverTest, ConstructServer) {
    config_manager = ConfigManager::makeConfigManager("server_test_config");
    test_server = new server(io_service, config_manager);
    using namespace std::chrono_literals;
    io_service.run_for(1s);
    exit(0);
}
// todo(yunqiu21): add tests for different cases
