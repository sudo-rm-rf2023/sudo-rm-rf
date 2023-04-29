#include "config_manager.h"
#include <fstream>
#include <optional>
#include <string>
#include <boost/numeric/conversion/cast.hpp>

bool StatementHasNTokens(const NginxConfigStatement& statement, uint32_t n){
  return statement.tokens_.size() == n;
}

bool StatementHasChildBlock(const NginxConfigStatement& statement){
  return statement.child_block_ != nullptr;
}

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

    // Check for required information: port


    return config_manager;
}

// Helper function that reads a Location block in the config;
// Return false if the block is invalid;
// Store a RouterEntry to memory and return true if successful.
// The request_path has to be non-empty, start with a '/', and does not end with a '/'. "/" is OK.
// The baseDir must start with a '/'.
// The location block must follow one of the following:
//  1. Have serveType = echo with no baseDir statement;
//  2. Have a baseDir statement;
//  Having serveType = echo with a baseDir statement present is invalid. 
//  Any Duplication is invalid.
bool ConfigManager::ReadLocation(const std::string& request_path, const NginxConfig& location_block){
  if (request_path.empty()){
    fprintf(stderr, "Location path is empty.");
    return false;
  };
  if (request_path[0] != '/') {
    fprintf(stderr, "The Location path should start with a '/'");
    return false;
  };
    if (request_path != "/" && request_path[request_path.size() - 1] == '/') {
    fprintf(stderr, "The Location path should not end with a '/'.");
    return false;
  };

  RouterEntry new_entry;
  new_entry.request_target = request_path;

  // duplication check
  bool handler_set = false;
  bool base_dir_set = false;

  for (std::shared_ptr<NginxConfigStatement> statement : location_block.statements_) {
    NginxConfigStatement* curr_statement = statement.get();
    if(!StatementHasNTokens(*curr_statement, 2)){
      fprintf(stderr, "serveType or baseDir statement requires two tokens.");
      return false;
    }
    std::string first_key_word = curr_statement->tokens_[0];
    std::string value = curr_statement->tokens_[1];
    if(first_key_word == CONFIG_KW_SERVETYPE){ // ReadServeType
        if (handler_set) {
          fprintf(stderr, "Duplicated serveType statement.");
          return false; // duplicated statement
        }
        handler_set = true;
        if(value == CONFIG_KW_ECHO){
          new_entry.handler_type = ECHO_HANDLER;
        } else if(value == CONFIG_KW_STATIC){
          new_entry.handler_type = STATIC_HANDLER;
        } else {
          fprintf(stderr, "Unknown serveType.");
          return false; // Unknown serveType
        }
    } else if (first_key_word == CONFIG_KW_BASEDIR){ // ReadbaseDir
        if (value.size() == 0 || value[0] != '/'){
          fprintf(stderr, "Invalid baseDir statement.");
          return false;
        }
        if (base_dir_set) {
          fprintf(stderr, "Duplicated baseDir statement.");
          return false; // duplicated statement
        }
        base_dir_set = true;
        new_entry.base_dir = value;

    } else {
        fprintf(stderr, "Unknown keyword %s", first_key_word.c_str());
        return false; // Unknown keyword.
    }

    if(base_dir_set && new_entry.handler_type == ECHO_HANDLER){ // Invalid Location Block
      fprintf(stderr, "Location block with Echo serveType should not have a baseDir statement.");
      return false;
    }

    // Turn to StaticHandler if baseDir is given
    if(base_dir_set){
      new_entry.handler_type = STATIC_HANDLER;
    }

  }
  // Store info in memory
  this->router_entries_.emplace_back(new_entry);
  return true;
}

// Helper function that reads a server block in the config.
// Return false if the block is invalid.
// Any Duplication is invalid
// Must have a listen statement
bool ConfigManager::ReadServer(const NginxConfig& server_block){

  // Helper function that reads the port in the config; Store the port number in memory.
  // Return false if the block is invalid;
  auto ReadListen = [&](const std::string& port_str) -> bool {
    try
    {
      this->port_number_ = boost::numeric_cast<short>(std::stoi(port_str));
      return true;
    }
    catch(const std::exception& e) // Catches failed stoi or numeric_cast
    {
      std::cerr << e.what() << '\n';
      return false;
    }
  };

  bool port_set = false;

  for(std::shared_ptr<NginxConfigStatement> statement : server_block.statements_){
    NginxConfigStatement* curr_statement = statement.get();
    if(!StatementHasNTokens(*curr_statement, 2)){
      fprintf(stderr, "Statement should have two tokens.");
      return false;
    }
    std::string first_key_word = curr_statement->tokens_[0];
    std::string value = curr_statement->tokens_[1];
    if(first_key_word == CONFIG_KW_LISTEN){
        if (port_set){
          fprintf(stderr, "Duplicated listen statement.");
          return false; // Duplication
        }
        if (!ReadListen(value)){
          return false;
        }
        port_set = true;
    } else if (first_key_word == CONFIG_KW_LOCATION){
        if(!StatementHasChildBlock(*curr_statement) ||
          !ReadLocation(value, *(curr_statement->child_block_.get()))){
          return false;
        }
    } else { // unknown keyword, print it out and skip
        printf("%s", curr_statement->ToString(0).c_str());
    }
  }
  return port_set;
}

// Must have a server block.
// Duplication is invalid.
bool ConfigManager::ReadConfig() {
  bool server_block_seen = false;
  for (std::shared_ptr<NginxConfigStatement> statement : config_.statements_){
    NginxConfigStatement* curr_statement = statement.get();
    if(StatementHasNTokens(*curr_statement, 0)){
      fprintf(stderr, "Empty statement is not allowed.");
      return false;
    }
    std::string first_key_word = curr_statement->tokens_[0];
    if(first_key_word == CONFIG_KW_SERVER){
      if(server_block_seen) return false;
      if(!StatementHasChildBlock(*curr_statement) ||
         !ReadServer(*(curr_statement->child_block_.get()))){
        return false;
      }
      server_block_seen = true;
    } else {
      printf("%s",curr_statement->ToString(0).c_str()); // Print out everything other than server block
    }
  }

  return server_block_seen;
}
