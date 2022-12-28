#ifndef VIRTUAL_FRAME_HPP
#define VIRTUAL_FRAME_HPP

#include <VulkanContext.hpp>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

class VirtualFrame {
 public:
  explicit VirtualFrame(const std::shared_ptr<VulkanContext>& context,
                        const vk::CommandBuffer& command_buffer);

  VirtualFrame(const VirtualFrame&) = delete;

  ~VirtualFrame();

  [[nodiscard]] const vk::CommandBuffer& GetCommandBuffer() const { return command_buffer; }
  [[nodiscard]] const vk::Semaphore& GetImageAvailable() const { return image_available; }
  [[nodiscard]] const vk::Semaphore& GetRenderFinished() const { return render_finished; }
  [[nodiscard]] const vk::Fence& GetInFlight() const { return in_flight; }

 private:
  std::shared_ptr<VulkanContext> context;

  vk::CommandBuffer command_buffer;

  vk::Semaphore image_available;
  vk::Semaphore render_finished;
  vk::Fence in_flight;
};

#endif  // VIRTUAL_FRAME_HPP
