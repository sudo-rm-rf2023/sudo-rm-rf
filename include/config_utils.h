#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include "utils.h"
#include "config_parser.h"
#include <vector>
#include <string>
#include <optional>
#include <memory>

namespace config_util {

std::optional<std::vector<DispatcherEntry>>  getDispatcherEntriesFromConfig(const NginxConfig& config);

std::optional<int> getPortFromConfig(const NginxConfig& config);

std::optional<std::string> getBaseDirFromLocationConfig(const NginxConfig& location_config);

} // namespace config_util

#endif
