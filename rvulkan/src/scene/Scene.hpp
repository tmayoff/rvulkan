#ifndef SCENE_HPP
#define SCENE_HPP

#include <Renderer.hpp>
#include <entt/entt.hpp>

#include "Core/types.hpp"
#include "VulkanContext.hpp"
#include "entt/entity/fwd.hpp"

class Entity;

class Scene {
  friend class Entity;

 public:
  explicit Scene(const std::shared_ptr<VulkanContext>& vulkan_context)
      : renderer(std::make_shared<Renderer>(vulkan_context, resolution_t{1904, 971})) {}

  Entity CreateEntity(const std::string& tag);

  void OnUpdate();

  void OnWindowResize(std::pair<float, float> size);

  [[nodiscard]] const entt::registry& GetRegistry() const { return registry; }

 private:
  entt::registry registry;
  std::shared_ptr<Renderer> renderer;
};

#endif  // SCENE_HPP
