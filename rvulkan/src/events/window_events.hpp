#ifndef WINDOW_EVENTS_HPP
#define WINDOW_EVENTS_HPP

#include <utility>

#include "event.hpp"

class WindowCloseEvent : public Event {
 public:
  WindowCloseEvent() = default;

  [[nodiscard]] DescriptorType_t Type() const override { return descriptor; }
  [[nodiscard]] static DescriptorType_t StaticType() { return descriptor; }

 private:
  static constexpr DescriptorType_t descriptor = "WindowCloseEvent";
};

class WindowResizeEvent : public Event {
 public:
  using Size_t = std::pair<uint32_t, uint32_t>;

  explicit WindowResizeEvent(Size_t size) : size(std::move(size)) {}

  [[nodiscard]] DescriptorType_t Type() const override { return descriptor; }
  [[nodiscard]] static DescriptorType_t StaticType() { return descriptor; }

  [[nodiscard]] const Size_t& GetSize() const { return size; }

 private:
  static constexpr DescriptorType_t descriptor = "WindowResizeEvent";

  Size_t size;
};

#endif  // WINDOW_EVENTS_HPP
