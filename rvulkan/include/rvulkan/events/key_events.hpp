#ifndef KEY_EVENTS_HPP
#define KEY_EVENTS_HPP

#include "event.hpp"
#include "keycodes.hpp"

class KeyEvent : public Event {
 public:
  explicit KeyEvent(Key key) : keycode(key) {}

  [[nodiscard]] Key GetKeycode() const { return keycode; }

 private:
  Key keycode;
};

#endif  // KEY_EVENTS_HPP
