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
#include "config_utils.h"

server::server(boost::asio::io_service &io_service, int port, Dispatcher* dispatcher, unsigned int num_threads) 
    : io_service_(io_service),
      dispatcher_(std::unique_ptr<Dispatcher>(dispatcher)),
      acceptor_(boost::asio::ip::tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), port))),
      num_threads_(num_threads),
      port_(port) {

    SetUpSignalHandlers();
    start_accept();
}

void server::run(){
    // Create a pool of worker threads to run the callback handlers
    std::vector<std::shared_ptr<boost::thread>> threads;
    for (unsigned int i = 0; i < num_threads_; ++i){
        // Each new thread runs io_service_.run() to register: https://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference/io_service/run/overload1.html
        std::shared_ptr<boost::thread> new_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
        threads.push_back(new_thread);
    }
    BOOST_LOG_TRIVIAL(info) << "Server Started. Number of worker threads: " << num_threads_ << "\n";
    BOOST_LOG_TRIVIAL(info) << "Port Number:" << port_ << "\n";

    for (std::shared_ptr<boost::thread> thread : threads){
        thread->join();
    }
}

void server::start_accept() {
    session *new_session = session::makeSession(io_service_, dispatcher_.get());
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
        BOOST_LOG_TRIVIAL(error) << "Accept Failed: " << error.message();
        delete new_session;
    }

    start_accept();
}

void server::Stop(){
    acceptor_.close();
    io_service_.stop();
    BOOST_LOG_TRIVIAL(info) << "Server Stopped.";
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
