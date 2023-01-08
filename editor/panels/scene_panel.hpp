#ifndef SCENE_PANEL_HPP
#define SCENE_PANEL_HPP

#include <memory>
#include <optional>
#include <rvulkan/scene/scene.hpp>
#include <utility>

class ScenePanel {
 public:
  explicit ScenePanel(std::shared_ptr<Scene> scene) : scene(std::move(scene)) {}

  void OnUpdate();

  [[nodiscard]] const std::optional<entt::entity>& GetSelectedContext() const {
    return selected_item;
  }

 private:
  void DrawEntity(const Entity& entity);

  std::optional<entt::entity> selected_item;

  std::shared_ptr<Scene> scene;
};

#endif  // SCENE_PANEL_HPP
