// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind.hpp>
#include <boost/beast/http.hpp>
#include <boost/optional/optional_io.hpp>
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
    std::string data_str(data_);
    int pos;
    if ((pos = data_str.find("\r\n\r\n")) == std::string::npos) {
      std::cerr << "Invalid header" << std::endl;
      delete this;
      return;
    }
    pos += 4;
    // parse the Content-Length field
    boost::beast::http::request_parser<boost::beast::http::string_body> parser;
    boost::beast::error_code ec;
    std::size_t bytes_parsed = parser.put(boost::asio::buffer(data_), ec);
    if (ec) {
      std::cerr << "Error parsing HTTP header: " << ec.message() << std::endl;
      delete this;
      return;
    }
    boost::optional<std::uint64_t> content_len = parser.content_length();
    if (!content_len.has_value()) {
      std::cerr << "Missing content length field" << std::endl;
      delete this;
      return;
    }
    // compose response
    int body_len = *content_len > bytes_transferred - pos ? bytes_transferred - pos : *content_len;
    std::string response_body = std::string(data_ + pos, body_len);
    std::ostringstream response_oss;
    response_oss << "HTTP/1.1 200 OK\r\n";
    response_oss << "Content-Type: text/plain\r\n";
    response_oss << "Content-Length: " << response_body.length() << "\r\n";
    response_oss << "\r\n";
    response_oss << response_body;
    std::string response_buf = response_oss.str();
    // write to socket
    boost::asio::async_write(socket_, boost::asio::buffer(response_buf),
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
