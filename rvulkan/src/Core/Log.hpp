#ifndef LOG_HPP
#define LOG_HPP

#include <spdlog/spdlog.h>

namespace logger {
void info(const std::string& msg);

///
/// @brief This will log as an error then throw an std::runtime_error
///
/// @param msg
///
void fatal(const std::string& msg);

}  // namespace logger

#endif  // LOG_HPP
