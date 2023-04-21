// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>

#include "server.h"

server::server(boost::asio::io_service &io_service, const char *config_file):io_service_(io_service),acceptor_(io_service) {

  // set up config manager and the smart pointer: https://learn.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-unique-ptr-instances
  config_manager_ = std::unique_ptr<ConfigManager>(ConfigManager::makeConfigManager(config_file));
  if (config_manager_ == nullptr) {
    // todo(@yunqiu21): change server constructor to factory method
    // and return nullptr here.
    throw std::invalid_argument("Invalid config file.");
  }

  acceptor_ = boost::asio::ip::tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), config_manager_->port()));

  printf("Port Number:%d\n", config_manager_->port());

  start_accept();
}

void server::start_accept()
{   
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
      const boost::system::error_code& error) {
    if (!error)
    {
      printf("New Connection.\n");
      new_session->start();
    }
    else
    {
      printf("Accept failed.\n");
      delete new_session;
    }

    start_accept();
  }
