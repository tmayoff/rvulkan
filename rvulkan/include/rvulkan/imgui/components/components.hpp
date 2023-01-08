#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <rvulkan/scene/components/tag.hpp>
#include <rvulkan/scene/components/transform.hpp>

#include "../imgui_utils.hpp"

inline bool DrawVec3(glm::vec3& vec3) {
  bool updated = false;

  if (ImGui::Button("X")) {
    updated = true;
    vec3.x = 0.0F;
  }

  ImGui::SameLine();

  if (ImGui::InputFloat("##X", &vec3.x)) updated = true;

  if (ImGui::Button("Y")) {
    updated = true;
    vec3.y = 0.0F;
  }
  ImGui::SameLine();
  if (ImGui::InputFloat("##Y", &vec3.y)) updated = true;

  if (ImGui::Button("Z")) {
    updated = true;
    vec3.z = 0.0F;
  }
  ImGui::SameLine();
  if (ImGui::InputFloat("##Z", &vec3.z)) updated = true;

  return updated;
}

inline void DrawTagComponent(Component::Tag& tag) {
  std::string tag_name = tag.GetTag();
  LabeledInputText("Tag", tag_name);
  tag.SetTag(tag_name);
}

inline void DrawTransformComponent(Component::Transform& transform) {
  glm::vec3 pos = transform.GetPosition();
  if (DrawVec3(pos)) transform.SetPosition(pos);
}

#endif  // COMPONENTS_HPP
