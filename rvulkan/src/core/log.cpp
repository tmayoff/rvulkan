#include <rvulkan/core/log.hpp>
#include <stdexcept>

#include "spdlog/common.h"
#include "spdlog/spdlog.h"

void logger::Init() { spdlog::set_level(spdlog::level::debug); }
