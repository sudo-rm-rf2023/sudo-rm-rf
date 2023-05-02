// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind/bind.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "server.h"
#include "logger.h"

server::server(boost::asio::io_service &io_service, ConfigManager *config_manager, Router *router) : io_service_(io_service), acceptor_(io_service) {

    config_manager_ = std::unique_ptr<ConfigManager>(config_manager);
    router_ = std::unique_ptr<Router>(router);
    acceptor_ = boost::asio::ip::tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), config_manager_->port()));

    SetUpSignalHandlers();
    BOOST_LOG_TRIVIAL(info) << "Port Number:" << config_manager_->port() << "\n";
    start_accept();
}

void server::start_accept() {
    session *new_session = session::makeSession(io_service_, router_.get());
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&server::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
}

void server::handle_accept(session *new_session,
                           const boost::system::error_code &error) {
    if (!error) {
        BOOST_LOG_TRIVIAL(info) << "New Connection.";
        new_session->start();
    } else {
        BOOST_LOG_TRIVIAL(error) << "Accept failed.";
        delete new_session;
    }

    start_accept();
}

void server::Stop(){
    acceptor_.close();
    io_service_.stop();
    printf("Server stopped.\n");
}

// Helper function to set up signal handling for Server objects to exit gracefully upon SIGINT and SIGNTERM
void server::SetUpSignalHandlers(){
    signals_.async_wait([&](const boost::system::error_code& ec, int signal_number){
        if (!ec) {
            printf("Received signal: %d. Exiting Gracefully...\n", signal_number);
            Stop();
        } else {
            printf("Error in signal handler: %s. Signal Number: %d.\n",ec.message().c_str(), signal_number);
        }
    });
}
