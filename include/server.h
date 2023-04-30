// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDE_SERVER
#define INCLUDE_SERVER
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>

#include "session.h"
#include "config_manager.h"

using boost::asio::ip::tcp;

class server {
public:
  server(boost::asio::io_service& io_service, ConfigManager* config_manager);
  server(boost::asio::io_service& io_service, short port) // Deprecate this
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)){
    start_accept();
  }
private:
  void SetUpSignalHandlers();
  void start_accept();

  void handle_accept(session* new_session,
      const boost::system::error_code& error);

  void Stop(); // Gracefully exit

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  std::unique_ptr<ConfigManager> config_manager_;

  boost::asio::signal_set signals_ = boost::asio::signal_set(io_service_, SIGINT, SIGTERM);
};

#endif