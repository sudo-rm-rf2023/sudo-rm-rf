// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "server.h"

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: async_tcp_echo_server <config_file>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        using namespace std; // For atoi.
        ConfigManager *config_manager = ConfigManager::makeConfigManager(argv[1]);
        if (config_manager == nullptr) {
            std::cerr << "Invalid config file\n";
            return 1;
        }
        server s(io_service, config_manager);
        io_service.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
