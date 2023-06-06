#include "session.h"
#include "gtest/gtest.h"
#include "router.h"

class sessionTest : public ::testing::Test {
protected:
    sessionTest() {}
    ~sessionTest() {
        delete test_session;
    }
    boost::asio::io_service io_service;
    boost::asio::ssl::context ssl_context{boost::asio::ssl::context::sslv23};
    session *test_session;
};

TEST_F(sessionTest, StartSession) {
    // Router router = Router(); // TODO: make this more elegant.
    std::unordered_map<std::string, RequestHandlerFactory*> routes;
    Dispatcher* dispatcher = new Dispatcher(routes);
    test_session = session::makeSession(io_service, ssl_context, dispatcher);
    test_session->start();
    return;
}
// todo(yunqiu21): add tests for different cases
