#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <cstdlib>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/status.hpp>



class RequestHandler {
    public:
        virtual int handle_request(
        const boost::beast::http::request<boost::beast::http::string_body>& request,
        boost::beast::http::response<boost::beast::http::string_body>& response) = 0;
};


#endif