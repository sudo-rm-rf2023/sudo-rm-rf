// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind/bind.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "server.h"

server::server(boost::asio::io_service &io_service, ConfigManager *config_manager) : io_service_(io_service), acceptor_(io_service) {

    // set up config manager and the smart pointer: https://learn.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-unique-ptr-instances
    config_manager_ = std::unique_ptr<ConfigManager>(config_manager);
    acceptor_ = boost::asio::ip::tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), config_manager_->port()));

    printf("Port Number:%d\n", config_manager_->port());

    start_accept();
}

void server::start_accept() {
    RequestHandler *echo_request_handler = new EchoRequestHandler();
    std::shared_ptr<session> new_session = std::shared_ptr<session>(session::makeSession(io_service_, echo_request_handler));
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&server::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
}

void server::handle_accept(std::shared_ptr<session> new_session,
                           const boost::system::error_code &error) {
    if (!error) {
        printf("New Connection.\n");
        new_session->start();
    } else {
        printf("Accept failed.\n");
    }

    start_accept();
}
