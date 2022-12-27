#ifndef EVENT_HPP
#define EVENT_HPP

#include <functional>
#include <string>

class Dispatcher;

class Event {
  friend class Dispatcher;

 public:
  virtual ~Event() = default;

  using DescriptorType_t = std::string_view;
  [[nodiscard]] virtual DescriptorType_t Type() const = 0;

  [[nodiscard]] bool Handled() const { return handled; }

 private:
  bool handled = false;
};

class Dispatcher {
 public:
  explicit Dispatcher(Event& e) : event(e) {}

  template <typename EventType>
  bool Dispatch(std::function<bool(EventType&)>&& fn) {
    if (event.Type() == EventType::StaticType()) {
      event.handled = fn(static_cast<EventType&>(event));
      return true;
    }

    return false;
  }

 private:
  Event& event;
};

#endif  // EVENT_HPP
