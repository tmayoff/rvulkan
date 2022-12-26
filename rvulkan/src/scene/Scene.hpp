#ifndef SCENE_HPP
#define SCENE_HPP

#include <entt/entt.hpp>

#include "entt/entity/fwd.hpp"

class Entity;

class Scene {
  friend class Entity;

 public:
  Entity CreateEntity(const std::string& tag);

  void OnUpdate();

  [[nodiscard]] const entt::registry& GetRegistry() const { return registry; }

 private:
  entt::registry registry;
};

#endif  // SCENE_HPP
