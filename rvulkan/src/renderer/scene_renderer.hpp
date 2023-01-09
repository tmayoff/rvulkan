#ifndef SCENE_RENDERER_HPP
#define SCENE_RENDERER_HPP

#include <memory>
#include <rvulkan/vulkan_context.hpp>

class SceneRenderer {
 public:
  explicit SceneRenderer(std::shared_ptr<VulkanContext> vulkan_context_);

 private:
  std::shared_ptr<VulkanContext> vulkan_context;
};

#endif  // SCENE_RENDERER_HPP
