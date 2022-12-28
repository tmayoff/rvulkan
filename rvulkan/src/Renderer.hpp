#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <vulkan/vulkan_core.h>

#include <Core/types.hpp>
#include <glm/glm.hpp>
#include <renderer/swapchain.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Buffer.hpp"
#include "RenderPass.hpp"
#include "VulkanContext.hpp"
#include "scene/Components/MeshRenderer.hpp"

const int MAX_FRAMES_IN_FLIGHT = 3;

class Renderer {
 public:
  explicit Renderer(const std::shared_ptr<VulkanContext>& context, const resolution_t& resolution);

  void StartFrame(const glm::mat4& view_projection);
  void EndFrame();

  void ResizeViewport(resolution_t size);

  void DrawMesh(const Component::MeshRenderer& mesh_renderer);

 private:
  void CreateRenderPass();
  void CreateCommandBuffers();
  void CreateSyncObjects();

  std::shared_ptr<VulkanContext> context;

  struct UniformBufferData {
    glm::mat4 view_projection{1.0F};
  } uniform_buffer_data;

  std::shared_ptr<Buffer> uniform_buffer;

  Swapchain swapchain;
  std::shared_ptr<RenderPass> renderPass;

  uint32_t current_frame_index = 0;
  uint32_t present_image_index = 0;

  bool view_resized = false;
  vk::Extent2D surface_extent;

  std::vector<vk::CommandBuffer> command_buffers;
  std::vector<vk::Semaphore> image_available_semaphores;
  std::vector<vk::Semaphore> render_finished_semaphores;
  std::vector<vk::Fence> in_flight_fences;
};

#endif  // RENDERER_HPP_
