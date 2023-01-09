#ifndef RENDER_CONTEXT_HPP
#define RENDER_CONTEXT_HPP

#include <rvulkan/core/types.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "render_pass.hpp"
#include "swapchain.hpp"

const int MAX_FRAMES_IN_FLIGHT = 2;

class RenderContext : public non_copyable, public non_movable {
 public:
  RenderContext() = default;
  RenderContext(const std::shared_ptr<VulkanContext>& vulkan_context_,
                std::shared_ptr<RenderPass> present_render_pass_);
  ~RenderContext();

  void PrepareFrame();
  void PresentFrame();

  void PushConstants(void* data, size_t size) const;
  void BindVertexBuffer(uint32_t first_binding, const vk::Buffer& buffer,
                        const std::vector<uint64_t>& offsets) const;
  void BindIndexBuffer(const vk::Buffer& buffer) const;
  void DrawIndexed(uint32_t index_count) const;

  void Resize(const vk::Extent2D& extent) {
    surface_extent = extent;
    view_resized = true;
  }

  void RunOneTimeCommand(const std::function<void(vk::CommandBuffer&)>& command_sequence);

  [[nodiscard]] const std::unique_ptr<Swapchain>& GetSwapchain() const { return swapchain; }

  [[nodiscard]] const vk::Framebuffer& GetCurrentFrameBuffer() const {
    return swapchain->GetFramebuffers()[swapchain_image_index];
  }

  [[nodiscard]] const vk::CommandBuffer& GetCurrentCommandBuffer() const {
    return command_buffers[current_frame_index];
  }

 private:
  void CreateCommandBuffers();
  void CreateSyncObjects();

  std::shared_ptr<VulkanContext> vulkan_context;

  uint32_t current_frame_index = 0;
  uint32_t swapchain_image_index = 0;

  bool view_resized = false;
  vk::Extent2D surface_extent;

  std::unique_ptr<Swapchain> swapchain;
  std::shared_ptr<RenderPass> present_render_pass;

  vk::CommandBuffer one_time_cmd_buffer;
  std::vector<vk::CommandBuffer> command_buffers;
  std::vector<vk::Semaphore> image_available_semaphores;
  std::vector<vk::Semaphore> render_finished_semaphores;
  std::vector<vk::Fence> in_flight_fences;
};

#endif  // RENDER_CONTEXT_HPP
