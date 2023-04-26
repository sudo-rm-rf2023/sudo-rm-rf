#include "session.h"
#include "gtest/gtest.h"

class sessionTest : public ::testing::Test {
protected:
    sessionTest() {}
    ~sessionTest() {
        delete test_session;
    }
    boost::asio::io_service io_service;
    session *test_session;
    RequestHandler* echo_request_handler;
};

TEST_F(sessionTest, StartSession) {
    echo_request_handler = new EchoRequestHandler();
    test_session = session::makeSession(io_service, echo_request_handler);
    test_session->start();
    return;
}
// todo(yunqiu21): add tests for different cases