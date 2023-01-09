#include "scene_renderer.hpp"

SceneRenderer::SceneRenderer(std::shared_ptr<VulkanContext> vulkan_context_)
    : vulkan_context(std::move(vulkan_context_)) {
  //
}
