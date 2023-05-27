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
#include "static_request_handler.h"
#include "router.h"
#include "dispatcher.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

// TODO: Write a module dedicated to process received request and produce a response
class session {
public:
  session(boost::asio::io_service& io_service, Dispatcher* dispatcher) :socket_(io_service),
                                                                        dispatcher_(dispatcher),
                                                                        strand_(io_service) {}

  static session* makeSession(boost::asio::io_service& io_service, Dispatcher* dispatcher){
    return new session(io_service, dispatcher);
  }

  tcp::socket& socket();
  void start();

private:
  void read_request();
  void write_response(const std::shared_ptr<http::response<http::string_body>>& response);
  void reset();

  tcp::socket socket_;

  // Strand to ensure the session's callback handlers functions are not called concurrently
  // https://valelab4.ucsf.edu/svn/3rdpartypublic/boost/doc/html/boost_asio/reference/io_service__strand.html
  boost::asio::io_service::strand strand_;

  Dispatcher* dispatcher_;

  boost::beast::flat_buffer buffer_;

};

#endif