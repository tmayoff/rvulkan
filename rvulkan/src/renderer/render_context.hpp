#ifndef RENDER_CONTEXT_HPP
#define RENDER_CONTEXT_HPP

#include <RenderPass.hpp>
#include <VulkanContext.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "core/types.hpp"
#include "swapchain.hpp"

const int MAX_FRAMES_IN_FLIGHT = 6;

class RenderContext {
 public:
  explicit RenderContext(const std::shared_ptr<VulkanContext>& vulkan_context_);

  void BeginFrame();
  void EndFrame();

  void BindVertexBuffer(uint32_t first_binding, const vk::Buffer& buffer,
                        const std::vector<uint64_t>& offsets);
  void BindIndexBuffer(const vk::Buffer& buffer);
  void DrawIndexed(uint32_t index_count);

  void Resize(const resolution_t& size) {
    surface_extent = vk::Extent2D(size.first, size.second);
    view_resized = true;
  }

  [[nodiscard]] const std::shared_ptr<RenderPass>& GetRenderPass() const { return render_pass; }

 private:
  void CreateRenderPass();
  void CreateCommandBuffers();
  void CreateSyncObjects();

  std::shared_ptr<VulkanContext> vulkan_context;

  uint32_t current_frame_index = 0;
  uint32_t swapchain_image_index = 0;

  bool view_resized = false;
  vk::Extent2D surface_extent;

  Swapchain swapchain;
  std::shared_ptr<RenderPass> render_pass;

  std::vector<vk::CommandBuffer> command_buffers;
  std::vector<vk::Semaphore> image_available_semaphores;
  std::vector<vk::Semaphore> render_finished_semaphores;
  std::vector<vk::Fence> in_flight_fences;
};

#endif  // RENDER_CONTEXT_HPP
