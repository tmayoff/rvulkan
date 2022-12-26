#ifndef TAG_HPP
#define TAG_HPP

#include <string>
#include <utility>

#include "Base.hpp"

namespace Component {

class Tag : public Base {
 public:
  explicit Tag(std::string tag) : tag(std::move(tag)) {}

  std::string GetTag() { return tag; }
  void SetTag(std::string tag) { this->tag = std::move(tag); }

 private:
  std::string tag;
};

}  // namespace Component
#endif  // TAG_HPP
