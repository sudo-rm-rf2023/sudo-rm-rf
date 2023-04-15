#ifndef CONFIG_MANAGER
#define CONFIG_MANAGER

#include <iostream>
#include "config_parser.h"
class ConfigManager {
public:
    ConfigManager(std::istream* config_stream);
    bool isValid(){return is_valid_config_;}
    short port(){return port_number_;}
private:
    void ReadConfig(); //Read the config and store important information in memory.
    bool is_valid_config_ = true;
    short port_number_;
    NginxConfig config_;
};

#endif