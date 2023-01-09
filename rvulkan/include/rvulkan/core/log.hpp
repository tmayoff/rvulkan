#ifndef LOG_HPP
#define LOG_HPP

#include <spdlog/spdlog.h>

#include <string_view>
#include <utility>

class logger {
 public:
  static void Init(const spdlog::level::level_enum& level);

  static void SetLogLevel(const spdlog::level::level_enum& level);

  template <typename... Args>
  static void trace(const std::string_view& msg, Args... args);

  template <typename... Args>
  static void debug(const std::string_view& msg, Args... args);

  template <typename... Args>
  static void info(const std::string_view& msg, Args... args);

  template <typename... Args>
  static void warning(const std::string_view& msg, Args... args);

  template <typename... Args>
  static void error(const std::string_view& msg, Args... args);

  ///
  /// @brief This will log as an error then throw an std::runtime_error
  ///
  /// @param msg
  ///
  template <typename... Args>
  static void fatal(const std::string_view& msg, Args... args);
};

template <typename... Args>
inline void logger::trace(const std::string_view& msg, Args... args) {
  spdlog::trace(fmt::runtime(msg), std::forward<Args>(args)...);
}

template <typename... Args>
inline void logger::debug(const std::string_view& msg, Args... args) {
  spdlog::debug(fmt::runtime(msg), std::forward<Args>(args)...);
}

template <typename... Args>
inline void logger::info(const std::string_view& msg, Args... args) {
  spdlog::info(fmt::runtime(msg), std::forward<Args>(args)...);
}

template <typename... Args>
inline void logger::warning(const std::string_view& msg, Args... args) {
  spdlog::warn(fmt::runtime(msg), std::forward<Args>(args)...);
}

template <typename... Args>
inline void logger::error(const std::string_view& msg, Args... args) {
  spdlog::error(fmt::runtime(msg), std::forward<Args>(args)...);
}

template <typename... Args>
inline void logger::fatal(const std::string_view& msg, Args... args) {
  spdlog::error(fmt::runtime(msg), std::forward<Args>(args)...);
  throw std::runtime_error("");
}

#endif  // LOG_HPP
