#ifndef TAG_HPP
#define TAG_HPP

#include <string>
#include <utility>

#include "base.hpp"

namespace Component {

class Tag : public Base {
 public:
  explicit Tag(std::string tag) : tag(std::move(tag)) {}

  [[nodiscard]] const std::string& GetTag() const { return tag; }
  void SetTag(std::string tag) { this->tag = std::move(tag); }

 private:
  std::string tag;
};

}  // namespace Component
#endif  // TAG_HPP
