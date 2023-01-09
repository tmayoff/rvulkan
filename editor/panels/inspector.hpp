#ifndef INSPECTOR_HPP
#define INSPECTOR_HPP

#include <entt/entt.hpp>
#include <optional>
#include <rvulkan/scene/scene.hpp>
#include <utility>

class Inspector {
 public:
  explicit Inspector(std::shared_ptr<Scene> scene) : scene(std::move(scene)) {}
  void OnUpdate();

  void SetSelectedEntity(const std::optional<entt::entity> entity) { selected_entity = entity; }

 private:
  std::shared_ptr<Scene> scene;

  std::optional<entt::entity> selected_entity;
};

#endif  // INSPECTOR_HPP
