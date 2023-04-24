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
#include <boost/beast/core/flat_buffer.hpp>

#include "echo_request_handler.h"

using boost::asio::ip::tcp;

// TODO: Write a module dedicated to process received request and produce a response
class session {
public:
  ~session(){
    printf("Session deleted.\n");
  }
  session(boost::asio::io_service& io_service, RequestHandler* request_handler) :socket_(io_service), request_handler_(request_handler) {
    printf("Session created.\n");
  }

  static session* makeSession(boost::asio::io_service& io_service, RequestHandler* request_handler){
    return new session(io_service, request_handler);
  }

  tcp::socket& socket();
  void start();

private:
  void read_request();
  void write_response(const std::shared_ptr<boost::beast::http::response<boost::beast::http::string_body>>& response);
  void reset();

  tcp::socket socket_;


  // TODO (qianli): Change this for Dependency Injection
  std::unique_ptr<RequestHandler> request_handler_;

  boost::beast::flat_buffer buffer_;

};

#endif