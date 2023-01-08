#ifndef KEY_EVENTS_HPP
#define KEY_EVENTS_HPP

#include "event.hpp"
#include "key_codes.hpp"

class KeyEvent : public Event {
 public:
  explicit KeyEvent(Key key) : keycode(key) {}

  [[nodiscard]] Key GetKeycode() const { return keycode; }

 private:
  Key keycode;
};

class KeyTypedEvent : public Event {
 public:
  explicit KeyTypedEvent(std::string text) : text(std::move(text)) {}

  [[nodiscard]] const std::string& GetText() const { return text; }

  static DescriptorType_t StaticType() { return "KeyTypedEvent"; }
  [[nodiscard]] DescriptorType_t Type() const override { return "KeyTypedEvent"; }

 private:
  std::string text;
};

class KeyPressedEvent : public KeyEvent {
 public:
  explicit KeyPressedEvent(Key key) : KeyEvent(key) {}

  static DescriptorType_t StaticType() { return "KeyPressedEvent"; }
  [[nodiscard]] DescriptorType_t Type() const override { return "KeyPressedEvent"; }
};

class KeyReleasedEvent : public KeyEvent {
 public:
  explicit KeyReleasedEvent(Key key) : KeyEvent(key) {}

  static DescriptorType_t StaticType() { return "KeyReleasedEvent"; }
  [[nodiscard]] DescriptorType_t Type() const override { return "KeyReleasedEvent"; }
};

#endif  // KEY_EVENTS_HPP
