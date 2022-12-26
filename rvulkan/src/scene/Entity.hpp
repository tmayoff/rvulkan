#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cassert>
#include <type_traits>

#include "Components/Base.hpp"
#include "Scene.hpp"
#include "entt/entity/fwd.hpp"

class Entity {
 public:
  Entity(Scene* scene, entt::entity handle) : scene(scene), handle(handle) {}

  template <typename T>
  [[nodiscard]] bool HasComponent() const;

  template <typename T, typename... Args>
  T& AddComponent(Args&&... args);

 private:
  Scene* scene;
  entt::entity handle;
};

template <typename T>
inline bool Entity::HasComponent() const {
  return scene->GetRegistry().all_of<T>(handle);
}

template <typename T, typename... Args>
inline T& Entity::AddComponent(Args&&... args) {
  T& comp = scene->registry.emplace<T>(handle, std::forward<Args>(args)...);

  if constexpr (std::is_base_of_v<Component::Base, T>) {
    // component.entity = handle;
    // component.scene = scene;
    comp.OnComponentAdded();
  }

  return comp;
}

#endif  // ENTITY_HPP
