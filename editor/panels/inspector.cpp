#include "inspector.hpp"

#include <rvulkan/imgui/imgui_utils.hpp>
#include <rvulkan/scene/components/tag.hpp>
#include <rvulkan/scene/entity.hpp>

void Inspector::OnUpdate() {
  if (ImGui::Begin("Inspector")) {
    if (selected_entity) {
      Entity entity{scene.get(), selected_entity.value()};

      auto& tag = entity.GetComponent<Component::Tag>();
      std::string tag_name = tag.GetTag();
      LabeledInputText("Tag", tag_name);
      tag.SetTag(tag_name);
    }

    ImGui::End();
  }
}
