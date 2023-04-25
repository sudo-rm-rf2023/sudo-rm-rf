// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind/bind.hpp>
#include <boost/beast/http.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/beast/http/message.hpp>
#include "session.h"

tcp::socket& session::socket() {
    return socket_;
}

void session::start() {
  read_request();
}

// read request by http::async_read
void session::read_request() {

  std::shared_ptr<boost::beast::http::request<boost::beast::http::string_body>> request = std::make_shared<boost::beast::http::request<boost::beast::http::string_body>>();

  boost::beast::http::async_read(socket_, buffer_, *request,
    [this, request](boost::beast::error_code ec, std::size_t bytes_transferred){
      if(!ec){
        printf("Read %ld bytes. Body: %ld Bytes.\n", bytes_transferred, request->body().size());
        // Create a shared pointer to the response object (lifetime managed by the session)
        std::shared_ptr<boost::beast::http::response<boost::beast::http::string_body>> response = std::make_shared<boost::beast::http::response<boost::beast::http::string_body>>();
        if(request_handler_->handle_request(*request, *response)){
          printf("Response Generated.\n");
          write_response(response);
        } else {
          printf("Response Generation Failed.\n");
          boost::beast::http::status status = boost::beast::http::status::internal_server_error;
          response->result(status);
          write_response(response);
        }
      }else{
        // Check if the error is caused by a bad request
        if (ec == boost::beast::http::error::bad_target) {
          fprintf(stderr, "Bad Request: %s\n", ec.message().c_str());
          // Send a 400 Bad Request response
          std::shared_ptr<boost::beast::http::response<boost::beast::http::string_body>> response = std::make_shared<boost::beast::http::response<boost::beast::http::string_body>>();
          request_handler_->handle_bad_request(*response);
          write_response(response);
        } else {
          fprintf(stderr, "Error in async_read (I/O): %s\n", ec.message().c_str());
        }
        delete this;
        return;
      }
    });
}

void session::reset() {
  printf("Session reseted.\n");
  // read_request();
}

// Write response to socket
void session::write_response(const std::shared_ptr<boost::beast::http::response<boost::beast::http::string_body>>& response){
  // printf("Body: %ld Bytes.\n", response->body().size());
  boost::beast::http::async_write(socket_, *response,
    // wrap response in a lambda function to keep it alive
    [this, response](boost::beast::error_code ec, std::size_t byte_transferred){
      printf("Write %ld bytes. Body: %ld Bytes.\n", byte_transferred, response->body().size());
      this->reset();
    });
}

