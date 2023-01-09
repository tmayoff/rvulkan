#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vulkan/vulkan_core.h>

#include <debug/profiler.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <renderer/swapchain.hpp>
#include <rvulkan/core/types.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Buffer.hpp"
#include "renderer/render_context.hpp"
#include "renderer/render_pass.hpp"

namespace Component {
class MeshRenderer;
}

class Renderer {
 public:
  explicit Renderer(std::shared_ptr<VulkanContext> vulkan_context_);

  [[nodiscard]] const std::shared_ptr<RenderContext>& GetRenderContext() const {
    return render_context;
  }

  void BeginFrame();
  void EndFrame();

  void ResizeViewport(resolution_t size);

  static void DrawMesh(const std::shared_ptr<RenderContext>& render_context,
                       const Component::MeshRenderer& mesh_renderer, const glm::mat4& transform);

 private:
  std::shared_ptr<VulkanContext> vulkan_context;
  std::shared_ptr<RenderContext> render_context;

  std::shared_ptr<RenderPass> present_render_pass;

  vk::Extent2D surface_extent;
};

inline void Renderer::ResizeViewport(resolution_t size) {
  surface_extent = vk::Extent2D(size.first, size.second);
  render_context->Resize(surface_extent);
}

#endif  // RENDERER_HPP
