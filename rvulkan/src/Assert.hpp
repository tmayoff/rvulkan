#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "Core.hpp"

#define ENABLE_ASSERTS
#ifdef ENABLE_ASSERTS

#define INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
  {                                                 \
    if (!(check)) {                                 \
      type##ERROR(msg, __VA_ARGS__);                \
      DEBUG_BREAK();                                \
    }                                               \
  }

#define INTERNAL_ASSERT_WITH_MSG(type, check, ...) \
  INTERNAL_ASSERT_IMPL(type, checl, "Assertion failed: %s", __VA_ARGS__)

#define INTERNAL_ASSERT_NO_MSG(type, check)                                               \
  INTERNAL_ASSERT_IMPL(type, check, "Assertion '%s' failed at: ", STRINGIFY_MACRO(check), \
                       std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro

#define INTERNAL_ASSERT_GET_MACRO(...)                                               \
  EXPAND_MACRO(INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, INTERNAL_ASSERT_WITH_MSG, \
                                              INTERNAL_ASSERT_NO_MSG))

#define CORE_ASSERT(...) EXPAND_MACRO(INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))

#else
#define CORE_ASSERT(...)
#endif

#endif  // ASSERT_HPP
