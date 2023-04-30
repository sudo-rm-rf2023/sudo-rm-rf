// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "server.h"
#include "logger.h"


int main(int argc, char *argv[]) {

    // Initialize the logger
    logger_init();
    BOOST_LOG_TRIVIAL(info) << "Server started.";

    try {
        if (argc != 2) {
            BOOST_LOG_TRIVIAL(error) << "Usage: async_tcp_echo_server <config_file>";
            std::cerr << "Usage: async_tcp_echo_server <config_file>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        using namespace std; // For atoi.
        ConfigManager *config_manager = ConfigManager::makeConfigManager(argv[1]);
        if (config_manager == nullptr) {
            BOOST_LOG_TRIVIAL(error) << "Invalid config file";
            std::cerr << "Invalid config file\n";
            return 1;
        }
        server s(io_service, config_manager);
        io_service.run();
    } catch (std::exception &e) {
        BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
        std::cerr << "Exception: " << e.what() << "\n";
    }

    BOOST_LOG_TRIVIAL(info) << "Server shutting down";
    return 0;
}
