#include "config_manager.h"
#include <fstream>

ConfigManager* ConfigManager::makeConfigManager(const char *config_file) {
    std::ifstream config_file_stream;
    config_file_stream.open(config_file);
    if (config_file_stream.fail()) {
        return nullptr;
    }
    ConfigManager *config_manager = new ConfigManager();
    NginxConfigParser parser;
    if (!parser.Parse(&config_file_stream, &(config_manager->config_))) {
        config_file_stream.close();
        delete config_manager;
        return nullptr;
    }
    if (!config_manager->ReadConfig()) {
        delete config_manager;
        return nullptr;
    }
    config_file_stream.close();
    return config_manager;
}

bool GetPort(const NginxConfig& config, short* port_number){
  // Helper function to get the port number from a NginxConfigParser Object.
  // Find the field server.listen. Use the first one if duplication exists.
  // Return false if not found.
  // TODO(yiyangzhou123): allow config such as http.server.listen
  for(auto statement : config.statements_){
     NginxConfigStatement* curr_statement = statement.get();
    if(curr_statement->tokens_.size() == 1 && curr_statement->tokens_[0] == "server"){ // find keyword "server"
      if (curr_statement->child_block_.get()!=nullptr){
        for(auto child_statement : curr_statement->child_block_.get()->statements_){
          // find keyword "listen"
          if (child_statement->tokens_.size() == 2 && child_statement->tokens_[0] == "listen"){
            try
            {
              *port_number = (short)std::stoi(child_statement->tokens_[1]);
              return true;
            }
            catch(const std::exception& e) // Catches failed stoi
            {
              std::cerr << e.what() << '\n';
              return false;
            }

          }
        }
      }
    }
  }
  return false;
}

bool ConfigManager::ReadConfig() {
    // read port number
    if(!GetPort(config_, &port_number_)){
        return false;
    }
    return true;
    // TODO for future assignments: read other important specs.
}
