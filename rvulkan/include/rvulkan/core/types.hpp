#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <utility>

using resolution_t = std::pair<uint32_t, uint32_t>;

class non_copyable {  // NOLINT
 public:
  non_copyable() = default;
  non_copyable(const non_copyable&) = delete;
  non_copyable& operator=(const non_copyable&) = delete;
};

class non_movable {  // NOLINT
 public:
  non_movable() = default;
  non_movable(non_movable&&) = delete;
  non_movable& operator=(non_movable&&) = delete;
};

#endif  // TYPES_HPP
