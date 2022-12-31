#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <vulkan/vulkan_core.h>

#include <Core/types.hpp>
#include <debug/profiler.hpp>
#include <glm/glm.hpp>
#include <renderer/swapchain.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Buffer.hpp"
#include "RenderPass.hpp"
#include "VulkanContext.hpp"
#include "renderer/render_context.hpp"
#include "scene/Components/MeshRenderer.hpp"

class Renderer {
 public:
  explicit Renderer(const std::shared_ptr<VulkanContext>& context);

  void BeginFrame(const glm::mat4& view_projection);
  void EndFrame();

  void ResizeViewport(resolution_t size);

  void DrawMesh(const Component::MeshRenderer& mesh_renderer);

 private:
  std::shared_ptr<VulkanContext> vulkan_context;
  RenderContext render_context;

  struct UniformBufferData {
    glm::mat4 view_projection{1.0F};
  } uniform_buffer_data;

  std::shared_ptr<Buffer> uniform_buffer;
};

inline void Renderer::ResizeViewport(resolution_t size) { render_context.Resize(size); }

#endif  // RENDERER_HPP_
