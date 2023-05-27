#include "config_utils.h"
#include "logger.h"
#include <boost/numeric/conversion/cast.hpp>

namespace {// Helper Functions
bool StatementHasNTokens(const NginxConfigStatement &statement, uint32_t n) {
    return statement.tokens_.size() == n;
}

bool StatementHasChildBlock(const NginxConfigStatement &statement) {
    return statement.child_block_ != nullptr;
}

std::optional<std::string> GetFirstTokenOfStatement(const NginxConfigStatement &statement) {
    return statement.tokens_.size() >= 1 ? std::optional<std::string>(statement.tokens_[0]) : std::nullopt;
}

// Index starts from 1.
std::optional<std::string> GetNthTokenOfStatement(const NginxConfigStatement &statement, uint32_t n) {
    if (n < 1) {
        return std::nullopt;
    }
    return statement.tokens_.size() >= n ? std::optional<std::string>(statement.tokens_[n - 1]) : std::nullopt;
}

HandlerType GetHandlerTypeFromToken(std::string type_token) {
    if (type_token == "EchoHandler") {
        return HandlerType::ECHO_HANDLER;
    } else if (type_token == "StaticHandler") {
        return HandlerType::STATIC_HANDLER;
    } else if (type_token == "404Handler") {
        return HandlerType::NOTFOUND_HANDLER;
    } else if (type_token == "CRUDApiHandler") {
        return HandlerType::CRUD_API_HANDLER;
    } else if (type_token == "HealthRequestHandler") {
        return HandlerType::HEALTH_REQUEST_HANDLER;
    } else if (type_token == "BlockRequestHandler") {
        return HandlerType::BLOCK_REQUEST_HANDLER;
    } else {
        return HandlerType::UNDEFINED_HANDLER;
    }
}

bool ValidateLocationBlock(NginxConfig location_config, HandlerType type){
    switch (type) {
    case UNDEFINED_HANDLER:
        return false;
        break;
    case STATIC_HANDLER:
        if(!config_util::getBaseDirFromLocationConfig(location_config).has_value()){
            return false;
        }
        break;
    case CRUD_API_HANDLER:
        if(!config_util::getDataPathFromLocationConfig(location_config).has_value()){
            return false;
        }
        break;
    case BLOCK_REQUEST_HANDLER:
        if(!config_util::getBlockTimeFromLocationConfig(location_config).has_value()){
            return false;
        }
        break;
    default:
        break;
    }
    return true;
}

} // namespace

namespace config_util {

std::optional<std::vector<DispatcherEntry>> getDispatcherEntriesFromConfig(const NginxConfig &config) {
    std::vector<DispatcherEntry> dispatcherEntries;
    for (std::shared_ptr<NginxConfigStatement> statement : config.statements_) {
        if (GetFirstTokenOfStatement(*statement) == "location" && StatementHasNTokens(*statement, 3) && StatementHasChildBlock(*statement)) {
            DispatcherEntry new_entry;
            new_entry.location = GetNthTokenOfStatement(*statement, 2).value();
            new_entry.handler_type = GetHandlerTypeFromToken(GetNthTokenOfStatement(*statement, 3).value());
            if (new_entry.handler_type == UNDEFINED_HANDLER) {
                BOOST_LOG_TRIVIAL(error) << "Config is invalid: Unknown Handler Type.";
                return std::nullopt;
            }
            new_entry.location_config = *(statement->child_block_.get());
            dispatcherEntries.emplace_back(new_entry);
        }
    }
    return dispatcherEntries;
}

std::optional<int> getPortFromConfig(const NginxConfig &config) {
    for (std::shared_ptr<NginxConfigStatement> statement : config.statements_) {
        std::optional<std::string> first_token = GetFirstTokenOfStatement(*statement);
        if ((first_token == "port" || first_token == "listen") && StatementHasNTokens(*statement, 2)) {
            std::string port_token = GetNthTokenOfStatement(*statement, 2).value();
            short port_number;
            try {
                port_number = boost::numeric_cast<short>(std::stoi(port_token));
                return port_number;
            } catch (const std::exception &e) {
                return std::nullopt;
            }
        }
    }
    return std::nullopt;
}

std::optional<unsigned int> getNumThreadsFromConfig(const NginxConfig &config)
{
    for (std::shared_ptr<NginxConfigStatement> statement : config.statements_) {
        std::optional<std::string> first_token = GetFirstTokenOfStatement(*statement);
        if ((first_token == "num_threads") && StatementHasNTokens(*statement, 2)) {
            std::string num_threads_token = GetNthTokenOfStatement(*statement, 2).value();
            unsigned int num_threads;
            try {
                num_threads = boost::numeric_cast<unsigned int>(std::stoi(num_threads_token));
                return num_threads;
            } catch (const std::exception &e) {
                return std::nullopt;
            }
        }
    }
    return std::nullopt;
}

std::optional<std::string> getBaseDirFromLocationConfig(const NginxConfig &location_config) {
    for (std::shared_ptr<NginxConfigStatement> statement : location_config.statements_) {
        if (GetFirstTokenOfStatement(*statement) == "root" && StatementHasNTokens(*statement, 2)) {
            return GetNthTokenOfStatement(*statement, 2);
        }
    }
    return std::nullopt;
}

std::optional<std::string> getDataPathFromLocationConfig(const NginxConfig &location_config) {
    for (std::shared_ptr<NginxConfigStatement> statement : location_config.statements_) {
        if (GetFirstTokenOfStatement(*statement) == "data_path" && StatementHasNTokens(*statement, 2)) {
            return GetNthTokenOfStatement(*statement, 2);
        }
    }
    return std::nullopt;
}

std::optional<int> getBlockTimeFromLocationConfig(const NginxConfig &location_config) {
    for (std::shared_ptr<NginxConfigStatement> statement : location_config.statements_) {
        if (GetFirstTokenOfStatement(*statement) == "sleep_time" && StatementHasNTokens(*statement, 2)) {
            std::string block_time_token = GetNthTokenOfStatement(*statement, 2).value();
            int block_time;
            try {
                block_time = std::stoi(block_time_token);
                return block_time;
            } catch (const std::exception &e) {
                return std::nullopt;
            }
        }
    }
    return std::nullopt;
}

bool validateConfig(const NginxConfig &config){
    if(!getPortFromConfig(config).has_value()){ // Must have port statement
        return false;
    }

    for (std::shared_ptr<NginxConfigStatement> statement : config.statements_){ // Check all location statements
        if (GetFirstTokenOfStatement(*statement) == "location"){
            HandlerType handler_type = GetHandlerTypeFromToken(GetNthTokenOfStatement(*statement, 3).value_or("Undefined"));
            if (!StatementHasNTokens(*statement, 3) ||
                !StatementHasChildBlock(*statement) ||
                !ValidateLocationBlock(*(statement->child_block_.get()), handler_type)){
                return false;
            }
        }
    }

    return true;
}

} // namespace config_util