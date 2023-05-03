#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <cstdlib>
#include <string>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/status.hpp>

#include "utils.h"

class RequestHandler {
    public:
        virtual int handle_request(
        const boost::beast::http::request<boost::beast::http::string_body>& request,
        boost::beast::http::response<boost::beast::http::string_body>& response) = 0;

        int handle_bad_request(boost::beast::http::response<boost::beast::http::string_body>& response){
            response.result(boost::beast::http::status::bad_request);
            response.set(boost::beast::http::field::content_type, "text/plain");
            response.body() = "Invalid Request";
            response.prepare_payload();
            return 1;
        }

        virtual HandlerType type()=0;
        void set_request_path (std::string path) {
            request_path = path;
        }
        std::string get_request_path (){
            return request_path;
        }
    private: 
        std::string request_path;
};

#endif