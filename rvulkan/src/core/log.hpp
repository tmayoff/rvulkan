#ifndef LOG_HPP
#define LOG_HPP

#include <spdlog/spdlog.h>

#include <string_view>
#include <utility>

#include "spdlog/fmt/bundled/core.h"

class logger {
 public:
  static void Init();

  template <typename... Args>
  static void debug(const std::string_view& msg, Args... args);

  template <typename... Args>
  static void info(const std::string_view& msg, Args... args);

  ///
  /// @brief This will log as an error then throw an std::runtime_error
  ///
  /// @param msg
  ///
  template <typename... Args>
  static void fatal(const std::string_view& msg, Args... args);
};

template <typename... Args>
inline void logger::debug(const std::string_view& msg, Args... args) {
  spdlog::debug(fmt::runtime(msg), std::forward<Args>(args)...);
}

template <typename... Args>
inline void logger::info(const std::string_view& msg, Args... args) {
  spdlog::info(fmt::runtime(msg), std::forward<Args>(args)...);
}

template <typename... Args>
inline void logger::fatal(const std::string_view& msg, Args... args) {
  spdlog::error(fmt::runtime(msg), std::forward<Args>(args)...);
  throw std::runtime_error("");
}

#endif  // LOG_HPP
