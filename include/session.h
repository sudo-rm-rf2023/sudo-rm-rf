// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDE_SESSION
#define INCLUDE_SESSION
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

using boost::asio::ip::tcp;

// TODO: Write a module dedicated to process received request and produce a response
class session {
public:
  ~session(){
    printf("Session deleted.\n");
  }
  session(boost::asio::io_service& io_service) :socket_(io_service) {};
  tcp::socket& socket();
  void start();


private:
  void read_header();
  void read_body(uint32_t byte);
  void generate_response();
  void write_response();
  void reset();

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  std::string received_;

  // TODO: Change these to http::request and http::response
  std::string request_header_; // content of request header in format of string
  std::string request_body_;

  boost::beast::http::response<boost::beast::http::string_body> response_;

  boost::beast::http::status status_ = boost::beast::http::status::ok;
};

#endif