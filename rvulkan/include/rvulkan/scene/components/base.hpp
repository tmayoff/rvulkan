#ifndef BASECOMPONENT_HPP
#define BASECOMPONENT_HPP

namespace Component {

class Base {
 public:
  Base() = default;
  Base(const Base&) = default;
  Base(Base&&) = default;

  Base& operator=(const Base&) = default;
  Base& operator=(Base&&) = default;

  virtual ~Base() = default;

  virtual void OnComponentAdded() {}
};

}  // namespace Component
#endif  // BASECOMPONENT_HPP
