#ifndef MOUSE_EVENTS_HPP
#define MOUSE_EVENTS_HPP

#include <utility>

#include "event.hpp"
#include "mouse_codes.hpp"

class MouseMoveEvent : public Event {
 public:
  explicit MouseMoveEvent(std::pair<uint32_t, uint32_t> coord) : coord(std::move(coord)) {}

  [[nodiscard]] auto GetMouseCoord() const { return coord; }

  static DescriptorType_t StaticType() { return "MouseMoveEvent"; }
  [[nodiscard]] DescriptorType_t Type() const override { return "MouseMoveEvent"; }

 private:
  std::pair<uint32_t, uint32_t> coord;
};

class MouseButtonEvent : public Event {
 public:
  explicit MouseButtonEvent(MouseButton button) : button(button) {}

  [[nodiscard]] MouseButton GetMouseButton() const { return button; }

  static DescriptorType_t StaticType() { return "MouseButtonEvent"; }
  [[nodiscard]] DescriptorType_t Type() const override { return "MouseButtonEvent"; }

 private:
  MouseButton button;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
 public:
  explicit MouseButtonPressedEvent(MouseButton button) : MouseButtonEvent(button) {}

  static DescriptorType_t StaticType() { return "MouseButtonPressedEvent"; }
  [[nodiscard]] DescriptorType_t Type() const override { return "MouseButtonPressedEvent"; }
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
 public:
  explicit MouseButtonReleasedEvent(MouseButton button) : MouseButtonEvent(button) {}

  static DescriptorType_t StaticType() { return "MouseButtonReleasedEvent"; }
  [[nodiscard]] DescriptorType_t Type() const override { return "MouseButtonReleasedEvent"; }
};

#endif  // MOUSE_EVENTS_HPP
