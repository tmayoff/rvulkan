#ifndef RENDERER_HPP_
#define RENDERER_HPP_

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
#include "RenderPass.hpp"
#include "renderer/render_context.hpp"

namespace Component {
class MeshRenderer;
}

class Renderer {
 public:
  explicit Renderer(const std::shared_ptr<VulkanContext>& context);

  [[nodiscard]] const RenderContext& GetRenderContext() const { return render_context; }

  void BeginFrame();
  void EndFrame();

  void ResizeViewport(resolution_t size);

  static void DrawMesh(const RenderContext& render_context,
                       const Component::MeshRenderer& mesh_renderer, const glm::mat4& transform);

 private:
  std::shared_ptr<VulkanContext> vulkan_context;
  RenderContext render_context;
};

inline void Renderer::ResizeViewport(resolution_t size) { render_context.Resize(size); }

#endif  // RENDERER_HPP_
