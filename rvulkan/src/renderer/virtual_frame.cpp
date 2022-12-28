#include "virtual_frame.hpp"

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

VirtualFrame::VirtualFrame(const std::shared_ptr<VulkanContext>& context,
                           const vk::CommandBuffer& command_buffer)
    : context(context), command_buffer(command_buffer) {
  const auto device = context->GetLogicalDevice().GetHandle();

  in_flight = device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));

  image_available = device.createSemaphore(vk::SemaphoreCreateInfo());
  render_finished = device.createSemaphore(vk::SemaphoreCreateInfo());
}

VirtualFrame::~VirtualFrame() {
  const auto device = context->GetLogicalDevice().GetHandle();

  device.freeCommandBuffers(context->GetCommandPool(), command_buffer);
  device.destroySemaphore(image_available);
  device.destroySemaphore(render_finished);
  device.destroyFence(in_flight);
}
