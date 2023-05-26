#ifndef LOGGER_H
#define LOGGER_H

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

const std::string RESPONSE_CODE = "ResponseCode: ";
const std::string REQUEST_PATH = "RequestPath: ";
const std::string REQUEST_METHOD = "RequestMethod: ";
const std::string REQUEST_IP = "RequestIP: ";
const std::string MATCHED_HANDLER = "MatchedHandler: ";


void logger_init();

#endif