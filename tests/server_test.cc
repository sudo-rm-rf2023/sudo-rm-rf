#include "config_manager.h"
#include "config_utils.h"
#include "server.h"
#include "session.h"
#include "gtest/gtest.h"
#include "router.h"

class serverTest : public ::testing::Test {
protected:
    serverTest():
        ssl_context(boost::asio::ssl::context::sslv23),
        dispatcher(nullptr),
        test_server(nullptr) {
        // Set up ssl context
        ssl_context.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::no_sslv3);
        ssl_context.use_certificate_chain_file("./test_ssl/cert.pem");
        ssl_context.use_private_key_file("./test_ssl/key.pem", boost::asio::ssl::context::pem);
    }  

    ~serverTest() {
        delete dispatcher;
        delete test_server;
    }

    boost::asio::io_service io_service;
    boost::asio::ssl::context ssl_context{boost::asio::ssl::context::sslv23};
    Dispatcher *dispatcher;
    std::unordered_map<std::string, RequestHandlerFactory*> routes;
    server *test_server;
};

TEST_F(serverTest, ConstructServer) {
    //TODO: make the port not hardcoded
    dispatcher = new Dispatcher(routes);
    test_server = new server(io_service, ssl_context, 80, dispatcher, /*num_thread=*/5);
    using namespace std::chrono_literals;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::loopback(), 80);
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket(io_service, ssl_context);
    socket.lowest_layer().connect(endpoint);
    io_service.run_for(1s);
    return;
}
// todo(yunqiu21): add tests for different cases
