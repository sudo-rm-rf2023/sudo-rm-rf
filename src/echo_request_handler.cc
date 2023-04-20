

#include "echo_request_handler.h"
#include <boost/beast/http/verb.hpp>
#include <sstream>


int EchoRequestHandler::handle_request(
    const boost::beast::http::request<boost::beast::http::string_body>& request, 
    boost::beast::http::response<boost::beast::http::string_body>& response) {

    response.version(request.version());
    response.result(boost::beast::http::status::ok);

    // TODO: delete this
    response.body() = request.body();
    response.set(boost::beast::http::field::content_type, "text/html");
    response.prepare_payload(); // set content-length
    return 1;

    // TODO (qianli): uncomment this

    // switch (status)
    // {
    // case boost::beast::http::status::ok:
    //     // 200 Response
    //     // Get the request method, target, and version as a string
    //     boost::beast::http::verb request_verb = request.method();
    //     std::string method_str = std::string(boost::beast::http::to_string(request_verb));
    //     std::string request_line = method_str + " " + request.target().to_string() + " HTTP/" + std::to_string(request.version() / 10) + "." + std::to_string(request.version() % 10);

    //     // Iterate through the fields in the request header and construct the header string
    //     std::string header_str = request_line + "\r\n";
    //     for (const auto& field : request.base()) {
    //         header_str += field.name_string().to_string() + ": " + field.value().to_string() + "\r\n";
    //     }
    //     header_str += "\r\n";
    //     response.body() = header_str + request.body();
    //     response.set(boost::beast::http::field::content_type, "text/html");
    //     break;
    // case boost::beast::http::status::bad_request:
    //     response.body() = "Invalid Request.\n";
    //     response.set(boost::beast::http::field::content_type, "text/html");
    //     break;
    
    // default:
    //     fprintf(stderr,"Unknown Error in generate_response.");
    //     break;
    // }
    // response.prepare_payload(); // set content-length
    // printf("Response Generated.\n");
    // return response;
}