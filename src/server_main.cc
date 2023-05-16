// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "server.h"
#include "logger.h"
#include "router.h"
#include "config_utils.h"
#include "dispatcher.h"
#include "config_parser.h"


int main(int argc, char *argv[]) {

    // Initialize the logger
    logger_init();
    BOOST_LOG_TRIVIAL(info) << "Server started.";

    try {
        if (argc != 2) {
            BOOST_LOG_TRIVIAL(error) << "Usage: async_tcp_echo_server <config_file>";
            return 1;
        }

        boost::asio::io_service io_service;

        
        NginxConfigParser parser;
        NginxConfig config;
        if (!parser.Parse(argv[1], &config)){
            BOOST_LOG_TRIVIAL(error) << "Invalid config file";
            return 1;
        }
        std::optional<int> port = config_util::getPortFromConfig(config);
        if (!port){
            BOOST_LOG_TRIVIAL(error) << "Invalid port configuration";
            return 1;
        }
        std::unordered_map<std::string, RequestHandlerFactory*> routes;
        if(!map_config_to_handler_factory(config, routes)){
            BOOST_LOG_TRIVIAL(error) << "Invalid handler configuration";
            return 1;
        }
        Dispatcher* dispatcher = new Dispatcher(routes);

        server s(io_service, port.value(), dispatcher);
        io_service.run();
    } catch (std::exception &e) {
        BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
    }

    BOOST_LOG_TRIVIAL(info) << "Server shutting down";
    return 0;
}
