// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDE_SESSION
#define INCLUDE_SESSION
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session {
public:
  session(boost::asio::io_service& io_service) :socket_(io_service) {};
  tcp::socket& socket();
  void start();


private:
  void handle_read(const boost::system::error_code& error,
                   size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

#endif