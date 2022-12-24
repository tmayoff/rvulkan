#include "Log.hpp"

#include <stdexcept>

#include "spdlog/spdlog.h"

namespace logger {

void info(const std::string& msg) { spdlog::info(msg); }

void fatal(const std::string& msg) {
  spdlog::error(msg);
  throw std::runtime_error("");
}

}  // namespace logger
