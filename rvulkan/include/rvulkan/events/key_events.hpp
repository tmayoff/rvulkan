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

class KeyPressedEvent : public KeyEvent {
 public:
  explicit KeyPressedEvent(Key key) : KeyEvent(key) {}
};

class KeyReleasedEvent : public KeyEvent {
  explicit KeyReleasedEvent(Key key) : KeyEvent(key) {}
};

#endif  // KEY_EVENTS_HPP
