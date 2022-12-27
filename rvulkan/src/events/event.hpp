#ifndef EVENT_HPP
#define EVENT_HPP

#include <functional>
#include <string>

class Dispatcher;

class Event {
  friend class Dispatcher;

 public:
  virtual ~Event() = default;

  using DescriptorType_t = const char*;
  [[nodiscard]] virtual DescriptorType_t Type() const = 0;

 private:
  bool handled = false;
};

class Dispatcher {
 public:
  explicit Dispatcher(Event& e) : event(e) {}

  template <typename EventType>
  bool Dispatch(std::function<bool(EventType&)>&& fn) {
    if (!event.handled) {
      event.handled = fn(static_cast<EventType&>(event));
      return true;
    }

    return false;
  }

 private:
  Event& event;
};

#endif  // EVENT_HPP
