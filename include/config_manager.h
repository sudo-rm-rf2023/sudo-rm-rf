#ifndef CONFIG_MANAGER
#define CONFIG_MANAGER

#include <iostream>
#include <string>
#include "config_parser.h"
#include "utils.h"

const std::string CONFIG_KW_SERVER = std::string("server");
const std::string CONFIG_KW_LISTEN = std::string("listen");
const std::string CONFIG_KW_LOCATION = std::string("location");
const std::string CONFIG_KW_SERVETYPE = std::string("serveType");
const std::string CONFIG_KW_BASEDIR = std::string("baseDir");
const std::string CONFIG_KW_ECHO = std::string("echo");
const std::string CONFIG_KW_STATIC = std::string("static");

class ConfigManager {
public:
    static ConfigManager* makeConfigManager(const char* config_file);
    short port(){return port_number_;}
    std::vector<RouterEntry> getRouterEntries(){return router_entries_;}
private:
    bool ReadConfig(); //Read the config and store important information in memory.

    short port_number_;
    std::vector<RouterEntry> router_entries_;
    NginxConfig config_;

     // Helper Functions for ReadConfig:
    bool ReadLocation(const std::string& request_path, const NginxConfig& location_block); // Read a Location Block
    bool ReadServer(const NginxConfig& server_block); // Read a server block


};

#endif