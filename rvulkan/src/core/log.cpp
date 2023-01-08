#include <rvulkan/core/log.hpp>
#include <stdexcept>

#include "spdlog/common.h"
#include "spdlog/spdlog.h"

void logger::Init(const spdlog::level::level_enum& level) { spdlog::set_level(level); }

void logger::SetLogLevel(const spdlog::level::level_enum& level) { spdlog::set_level(level); }
