#include "config_utils.h"
#include <boost/numeric/conversion/cast.hpp>
#include "logger.h"

// Helper Functions
bool StatementHasNTokens(const NginxConfigStatement& statement, uint32_t n){
  return statement.tokens_.size() == n;
}

bool StatementHasChildBlock(const NginxConfigStatement& statement){
  return statement.child_block_ != nullptr;
}

std::optional<std::string> GetFirstTokenOfStatement(const NginxConfigStatement& statement){
    return statement.tokens_.size() >= 1 ? std::optional<std::string>(statement.tokens_[0]) : std::nullopt;
}

// Index starts from 1.
std::optional<std::string> GetNthTokenOfStatement(const NginxConfigStatement& statement, uint32_t n){
    if(n < 1){
        return std::nullopt;
    }
    return statement.tokens_.size() >= n ? std::optional<std::string>(statement.tokens_[n - 1]) : std::nullopt;
}

HandlerType GetHandlerTypeFromToken(std::string type_token){
    if(type_token == "EchoHandler"){
        return HandlerType::ECHO_HANDLER;
    } else if (type_token == "StaticHandler") {
        return HandlerType::STATIC_HANDLER;
    } else if (type_token == "404Handler"){
        return HandlerType::NOTFOUND_HANDLER;
    } else {
        return HandlerType::UNDEFINED_HANDLER;
    }
}

namespace config_util
{

std::optional<std::vector<DispatcherEntry>> getDispatcherEntriesFromConfig(const NginxConfig& config){
    std::vector<DispatcherEntry> dispatcherEntries;
    for (std::shared_ptr<NginxConfigStatement> statement : config.statements_){
        if (GetFirstTokenOfStatement(*statement) == "location" && StatementHasNTokens(*statement, 3) && StatementHasChildBlock(*statement)){
            DispatcherEntry new_entry;
            new_entry.location = GetNthTokenOfStatement(*statement, 2).value();
            new_entry.handler_type = GetHandlerTypeFromToken(GetNthTokenOfStatement(*statement, 3).value());
            if(new_entry.handler_type == UNDEFINED_HANDLER){
                BOOST_LOG_TRIVIAL(error) << "Config is invalid: Unknown Handler Type.";
                return std::nullopt;
            }
            new_entry.location_config = *(statement->child_block_.get());
            dispatcherEntries.emplace_back(new_entry);
        }
    }
    return dispatcherEntries;
}

std::optional<int> getPortFromConfig(const NginxConfig &config){
    for (std::shared_ptr<NginxConfigStatement> statement : config.statements_){
        std::optional<std::string> first_token = GetFirstTokenOfStatement(*statement);
        if ((first_token == "port" || first_token == "listen") && StatementHasNTokens(*statement, 2)){
            std::string port_token = GetNthTokenOfStatement(*statement, 2).value();
            short port_number;
            try {
                port_number = boost::numeric_cast<short>(std::stoi(port_token));
                return port_number;
            } catch (const std::exception& e){
                return std::nullopt;
            }
        }
    }
    return std::nullopt;
}

std::optional<std::string> getBaseDirFromLocationConfig(const NginxConfig &location_config)
{
    for (std::shared_ptr<NginxConfigStatement> statement : location_config.statements_){
        if (GetFirstTokenOfStatement(*statement) == "root" && StatementHasNTokens(*statement, 2)){
            return GetNthTokenOfStatement(*statement, 2);
        }
    }
    return std::nullopt;
}


} // namespace config_util