#ifndef CONFIG_MANAGER
#define CONFIG_MANAGER

#include <iostream>
#include "config_parser.h"
class ConfigManager {
public:
    static ConfigManager* makeConfigManager(const char* config_file);
    short port(){return port_number_;}
private:
    bool ReadConfig(); //Read the config and store important information in memory.
    short port_number_;
    NginxConfig config_;
};

#endif