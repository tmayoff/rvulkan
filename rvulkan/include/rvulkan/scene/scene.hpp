#ifndef SCENE_HPP
#define SCENE_HPP

#include <entt/entt.hpp>

#include "entt/entity/fwd.hpp"

class RenderContext;
class Entity;

class Scene {
  friend class Entity;

 public:
  Entity CreateEntity(const std::string& tag);

  void OnUpdate(const std::shared_ptr<RenderContext>& render_context);

  void OnWindowResize(std::pair<float, float> size);

  [[nodiscard]] const entt::registry& GetRegistry() const { return registry; }

 private:
  entt::registry registry;
};

#endif  // SCENE_HPP
