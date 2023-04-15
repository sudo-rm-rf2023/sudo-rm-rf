// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind.hpp>

#include "session.h"

tcp::socket& session::socket() {
    return socket_;
}

void session::start() {
socket_.async_read_some(boost::asio::buffer(data_, max_length),
    boost::bind(&session::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error,
                          size_t bytes_transferred) {
  if (!error) {
    // parse header
    // TODO(yunqiu21): parse the Content-Length field
    std::string data_str(data_);
    int pos;
    if ((pos = data_str.find("\r\n\r\n")) == std::string::npos) {
      std::cerr << "Invalid header" << std::endl;
      delete this;
      return;
    }
    pos += 4;
    // compose response
    // TODO(yunqiu21): add a Content-Length field
    std::string response =
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" +
        std::string(data_ + pos, bytes_transferred - pos);
    // write to socket
    boost::asio::async_write(socket_, boost::asio::buffer(response),
                             boost::bind(&session::handle_write, this,
                             boost::asio::placeholders::error));
  } else {
    delete this;
  }
}

void session::handle_write(const boost::system::error_code& error) {
    if (!error) {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else {
        delete this;
    }
}
