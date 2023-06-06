// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind/bind.hpp>
#include <boost/beast/http.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/beast/http/message.hpp>
#include "session.h"
#include "logger.h"

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

boost::asio::ssl::stream<tcp::socket>& session::socket() {
    return socket_;
}

void session::start() {
  // Perform the SSL handshake
  socket_.async_handshake(boost::asio::ssl::stream_base::server,
    strand_.wrap(
      [this](boost::beast::error_code ec){
        if(!ec){
          read_request();
        } else {
          BOOST_LOG_TRIVIAL(error) << "Error in async_handshake (I/O): " << ec.message();
          delete this;
          return;
        }
      }));
}

// read request by http::async_read
void session::read_request() {
  http::async_read(socket_, buffer_, request_, strand_.wrap(
    [this](boost::beast::error_code ec, std::size_t bytes_transferred){
      if(!ec){
        BOOST_LOG_TRIVIAL(info) << REQUEST_IP << socket_.lowest_layer().remote_endpoint().address().to_string() << ":" << socket_.lowest_layer().remote_endpoint().port();
        // generate error message if failed to generate response
        if(!dispatcher_->assign_request(request_, response_)){
          BOOST_LOG_TRIVIAL(error) << "Failed to generate response.";
          response_.result(http::status::internal_server_error);
        }
        write_response();
      }else{
        if (ec == http::error::bad_target) {
          BOOST_LOG_TRIVIAL(error) << "Bad request";
          // Send a 400 Bad Request response
          dispatcher_->handle_bad_request(response_);
          http::write(socket_, response_);
        } else if (ec == http::error::end_of_stream) {
          BOOST_LOG_TRIVIAL(info) << "End of Stream received. The Client may have closed the connection.";
        } else {
          BOOST_LOG_TRIVIAL(error) << "Error in async_read (I/O): " << ec.message();
        }
        delete this;
        return;
      }
    }));
}

void session::reset() {
  request_.clear();
  request_.body().clear();
  response_.clear();
  response_.body().clear();
  read_request();
}

// Write response to socket
void session::write_response(){
  http::async_write(socket_, response_, strand_.wrap(
    [this](boost::beast::error_code ec, std::size_t byte_transferred){
      this->reset();
    }));
}

