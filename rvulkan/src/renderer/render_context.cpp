#include "render_context.hpp"

#include <debug/profiler.hpp>
#include <memory>
#include <rvulkan/core/log.hpp>
#include <rvulkan/renderer/mesh.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

RenderContext::RenderContext(const std::shared_ptr<VulkanContext>& vulkan_context_,
                             std::shared_ptr<RenderPass> present_render_pass_)
    : vulkan_context(vulkan_context_), present_render_pass(std::move(present_render_pass_)) {
  swapchain = std::make_unique<Swapchain>(vulkan_context_, present_render_pass);

  CreateCommandBuffers();
  CreateSyncObjects();
}

RenderContext::~RenderContext() {
  swapchain.reset();

  const auto& device = vulkan_context->GetLogicalDevice()->GetHandle();

  device.freeCommandBuffers(vulkan_context->GetCommandPool(), command_buffers);
  for (const auto& semaphore : image_available_semaphores) device.destroySemaphore(semaphore);
  for (const auto& semaphore : render_finished_semaphores) device.destroySemaphore(semaphore);
  for (const auto& fence : in_flight_fences) device.destroyFence(fence);

  present_render_pass.reset();
}

void RenderContext::PrepareFrame() {
  ZoneScoped;  // NOLINT

  const auto device = vulkan_context->GetLogicalDevice()->GetHandle();
  {
    ZoneScopedN("Wait for Fence");  // NOLINT
    vk::Result wait_for_fence =
        device.waitForFences(in_flight_fences[current_frame_index], VK_FALSE, UINT64_MAX);
    if (wait_for_fence != vk::Result::eSuccess) logger::fatal("Failed to wait for fence");
  }

  {
    ZoneScopedN("Acquire Swapchain Image");  // NOLINT

    // Acquire Swapchain Image
    const auto acquired_image_index = device.acquireNextImageKHR(
        swapchain->GetHandle(), UINT64_MAX, image_available_semaphores[current_frame_index]);
    if (acquired_image_index.result == vk::Result::eErrorOutOfDateKHR) {
      swapchain->RecreateSwapchain(surface_extent);
      return;
    }
    swapchain_image_index = acquired_image_index.value;
  }

  device.resetFences(in_flight_fences[current_frame_index]);
}

void RenderContext::PresentFrame() {
  ZoneScoped;  // NOLINT

  const auto device = vulkan_context->GetLogicalDevice();

  command_buffers[current_frame_index].endRenderPass();
  command_buffers[current_frame_index].end();

  std::array wait_stages = {
      vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)};

  auto wait_semaphore = image_available_semaphores[current_frame_index];
  auto signal_semaphore = render_finished_semaphores[current_frame_index];

  {
    ZoneScopedN("Queue Submit");  // NOLINT
    vk::SubmitInfo submit_info(wait_semaphore, wait_stages, command_buffers[current_frame_index],
                               signal_semaphore);

    device->GetGraphicsQueue().submit(submit_info, in_flight_fences[current_frame_index]);
  }

  vk::PresentInfoKHR present_info(signal_semaphore, swapchain->GetHandle(), swapchain_image_index);

  try {
    ZoneScopedN("Queue Present");  // NOLINT
    auto res = device->GetPresentQueue().presentKHR(present_info);
    if (res == vk::Result::eSuboptimalKHR || view_resized) {
      view_resized = false;
      swapchain->RecreateSwapchain(surface_extent);
    }
  } catch (vk::OutOfDateKHRError& e) {
    view_resized = false;
    swapchain->RecreateSwapchain(surface_extent);
  }

  current_frame_index = (current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RenderContext::PushConstants(void* data, size_t size) const {
  command_buffers[current_frame_index].pushConstants(
      present_render_pass->GetPipeline()->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, size,
      data);
}

// void RenderContext::BindDescriptorSet() {
//   command_buffers[current_frame_index].bindDescriptorSets(
//       vk::PipelineBindPoint::eGraphics, render_pass->GetPipeline()->GetLayout(), 0,
//       render_pass->GetPipeline()->GetDescriptorSets(), nullptr);
// }

void RenderContext::BindVertexBuffer(uint32_t first_binding, const vk::Buffer& buffer,
                                     const std::vector<uint64_t>& offsets) const {
  command_buffers[current_frame_index].bindVertexBuffers(first_binding, buffer, offsets);
}

void RenderContext::BindIndexBuffer(const vk::Buffer& buffer) const {
  command_buffers[current_frame_index].bindIndexBuffer(buffer, 0, vk::IndexType::eUint32);
}

void RenderContext::DrawIndexed(uint32_t index_count) const {
  command_buffers[current_frame_index].drawIndexed(index_count, 1, 0, 0, 0);
}

void RenderContext::CreateCommandBuffers() {
  command_buffers.reserve(MAX_FRAMES_IN_FLIGHT);

  vk::CommandBufferAllocateInfo alloc_info(vulkan_context->GetCommandPool(),
                                           vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
  command_buffers =
      vulkan_context->GetLogicalDevice()->GetHandle().allocateCommandBuffers(alloc_info);
}

void RenderContext::CreateSyncObjects() {
  auto device = vulkan_context->GetLogicalDevice()->GetHandle();

  image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

  for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    image_available_semaphores[i] = device.createSemaphore(vk::SemaphoreCreateInfo());
    render_finished_semaphores[i] = device.createSemaphore(vk::SemaphoreCreateInfo());
    in_flight_fences[i] =
        device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
  }
}
