#include "scene_panel.hpp"

#include <rvulkan/core/log.hpp>
#include <rvulkan/scene/components/tag.hpp>
#include <rvulkan/scene/entity.hpp>

#include "imgui.h"

void ScenePanel::OnUpdate() {
  ImGui::Begin("Scene");

  // Populate scene hirearchy
  scene->GetRegistry().each([this](auto entity_handle) {
    Entity entity{scene.get(), entity_handle};
    DrawEntity(entity);
  });

  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::MenuItem("Create Empty Entity")) scene->CreateEntity("New Entity");
    ImGui::EndPopup();
  }

  if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) selected_item.reset();

  ImGui::End();
}

void ScenePanel::DrawEntity(const Entity& entity) {
  const auto& tag = entity.GetComponent<Component::Tag>();

  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
  if (selected_item == entity.GetHandle()) flags |= ImGuiTreeNodeFlags_Selected;

  bool opened = ImGui::TreeNodeEx((void*)entity.GetHandle(), flags, "%s", tag.GetTag().c_str());

  if (ImGui::IsItemClicked()) selected_item = entity.GetHandle();

  if (opened) ImGui::TreePop();
}
