#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cassert>
#include <type_traits>

#include "components/base.hpp"
#include "components/transform.hpp"
#include "entt/entity/fwd.hpp"
#include "scene.hpp"

class Entity {
 public:
  Entity() = default;
  Entity(Scene* scene, entt::entity handle) : scene(scene), handle(handle) {}

  [[nodiscard]] Component::Transform& GetTransform() const {
    return GetComponent<Component::Transform>();
  }

  template <typename T>
  [[nodiscard]] bool HasComponent() const;

  template <typename T>
  T& GetComponent() const;

  template <typename T, typename... Args>
  T& AddComponent(Args&&... args);

  [[nodiscard]] const entt::entity& GetHandle() const { return handle; }

 private:
  Scene* scene;
  entt::entity handle;
};

template <typename T>
inline bool Entity::HasComponent() const {
  return scene->registry.all_of<T>(handle);
}

template <typename T>
inline T& Entity::GetComponent() const {
  return scene->registry.get<T>(handle);
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
