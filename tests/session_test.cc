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
    session *test_session;
};

TEST_F(sessionTest, StartSession) {
    Router router = Router(); // TODO: make this more elegant.
    test_session = session::makeSession(io_service, &router);
    test_session->start();
    return;
}
// todo(yunqiu21): add tests for different cases
