#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>

#include "Base.hpp"

namespace Component {

class Transform : public Base {
 public:
  void SetPosition(const glm::vec3& pos) { position = pos; }
  void SetRotation(const glm::vec3& rot) { rotation = rot; }

  [[nodiscard]] const glm::vec3& GetPosition() const { return position; }
  [[nodiscard]] const glm::vec3& GetRotation() const { return rotation; }
  [[nodiscard]] const glm::vec3& GetScale() const { return scale; }

  [[nodiscard]] auto GetObjectToWorld() const -> glm::mat4;

 private:
  //   glm::mat4 object_to_world = glm::mat4{1.0F};

  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale = glm::vec3{1.0F};
};

class RectTransform : public Transform {};

}  // namespace Component
#endif  // TRANSFORM_HPP
