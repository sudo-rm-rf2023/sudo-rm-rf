#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include "config_parser.h"
#include "utils.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace config_util {

bool validateConfig(const NginxConfig &config);

std::optional<std::vector<DispatcherEntry>> getDispatcherEntriesFromConfig(const NginxConfig &config);

std::optional<int> getPortFromConfig(const NginxConfig &config);

std::optional<std::string> getBaseDirFromLocationConfig(const NginxConfig &location_config);

std::optional<std::string> getDataPathFromLocationConfig(const NginxConfig &location_config);

} // namespace config_util

#endif
