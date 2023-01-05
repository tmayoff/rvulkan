#include "transform.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Component {

auto Transform::GetObjectToWorld() const -> glm::mat4 {
  glm::mat4 pos = glm::translate(glm::mat4{1.0F}, position);
  glm::mat4 rot = glm::toMat4(glm::quat(glm::radians(rotation)));
  glm::mat4 sca = glm::scale(glm::mat4{1.0F}, scale);

  auto obj_to_world = pos * rot * sca;

  // Once a hierarchy is created add parent stuff here

  return obj_to_world;
}

}  // namespace Component
