#ifndef CONFIG_MANAGER
#define CONFIG_MANAGER

#include <iostream>
#include <string>
#include "config_parser.h"
#include "utils.h"
#include "config_utils.h"

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