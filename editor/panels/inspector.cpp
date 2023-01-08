#include "inspector.hpp"

#include <rvulkan/imgui/components/components.hpp>
#include <rvulkan/imgui/imgui_utils.hpp>
#include <rvulkan/scene/components/tag.hpp>
#include <rvulkan/scene/entity.hpp>

void Inspector::OnUpdate() {
  if (ImGui::Begin("Inspector")) {
    if (selected_entity) {
      Entity entity{scene.get(), selected_entity.value()};

      DrawTagComponent(entity.GetComponent<Component::Tag>());

      ImGui::Separator();

      DrawTransformComponent(entity.GetTransform());

      ImGui::Separator();
    }

    ImGui::End();
  }
}
