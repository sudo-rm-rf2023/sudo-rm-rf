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
  read_header();
}

void session::reset() {
  memset(data_, 0, max_length);
  received_.clear();
  request_header_.clear();
  request_body_.clear();
  response_.clear();
  
  printf("Session reseted.\n");
  read_header();
}

// Populate the member response_ based on status_.
void session::generate_response() {
  response_.version(11);
  response_.result(status_);
  switch (status_)
  {
  case boost::beast::http::status::ok:
    // 200 Response
    response_.body() = request_header_ + request_body_;
    response_.set(boost::beast::http::field::content_type, "text/html");
    break;
  case boost::beast::http::status::bad_request:
    response_.body() = "Invalid Request.\n";
    response_.set(boost::beast::http::field::content_type, "text/html");
    break;
  
  default:
    fprintf(stderr,"Unknown Error in generate_response.");
    break;
  }
  response_.prepare_payload(); // set content-length
  printf("Response Generated.\n");
  write_response();
}

// Write response to socket
void session::write_response(){
  boost::beast::http::async_write(socket_, response_, 
    [this](boost::beast::error_code ec, std::size_t byte_transferred){
      printf("Write %ld bytes. Body: %ld Bytes.\n", byte_transferred, response_.body().size());
      this->reset();
    });
}

// Keep reading the socket until \r\n\r\n is found. Update the member request_header_.
// Update the member status_ as needed.
// TODO: Combind read_header and read_body and Switch to http::async_read
void session::read_header(){
  
  //Helper function
  auto locate_header_end = [this](){
    int pos;
    if ((pos = received_.find("\r\n\r\n", 0)) != std::string::npos) { 
      return pos;
    } 
    else if ((pos = received_.find("\n\n", 0)) != std::string::npos) { 
      return pos;
    }else { 
      return -1;
    }
  };

  socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this,locate_header_end](boost::system::error_code error, std::size_t byte_transferred){
      if(!error){
        received_.append(data_, byte_transferred);
        int pos;
        if((pos = locate_header_end()) >= 0){ // If we find the end of request header
          request_header_ = received_.substr(0, pos + 4);
          request_body_ = received_.substr(pos + 4);
          // Get content-length
          boost::beast::http::request_parser<boost::beast::http::string_body> parser;
          std::size_t bytes_parsed = parser.put(boost::asio::buffer(data_), error);
          if(!parser.is_header_done()){
            fprintf(stderr, "Error in parsing header: header not done.\n");
            status_ = boost::beast::http::status::bad_request;
            this->generate_response();
            return;
          }

          boost::optional<std::uint64_t> content_len = parser.content_length();
          if (content_len.has_value()){ // Read message body if content-length is set
            printf("Header parsed. Content length: %ld\n", *content_len);
            std::size_t remainder_to_read = *content_len - request_body_.size();
            this->read_body(remainder_to_read);
          }
          else {
            printf("Header parsed. No content length.\n");
            this->generate_response();
          }
        } else { // If yet to find the end of request
          printf("Did not find the read_header. Continue to read.\n");
          read_header(); // Continue reading. TODO: set a timer.
        }
      } else {
        delete this;
        return;
      }
    });
}

// Read bytes from socket and append data to request_body_.
void session::read_body(uint32_t bytes){
  //printf("Begins reading body. \n");
  socket_.async_read_some(boost::asio::buffer(data_, bytes <= max_length ? bytes : max_length), // read min(bytes, max_length)
  [&](boost::system::error_code error, std::size_t byte_transferred){
    if(!error){
      //printf("Received %ld bytes.\n", byte_transferred);
      request_body_.append(data_, byte_transferred);
      bytes -= byte_transferred;
      if(bytes > 0){ // If there is more to read
        read_body(bytes);
      } else if (bytes == 0){
        printf("Finished reading body. Received: %ld bytes.", this->request_body_.size());
        generate_response();
      } else {
        fprintf(stderr, "Error in read body"); // Impossible error.
        reset();
      }
    } else {
      delete this;
      return;
    }
  });

  
}
