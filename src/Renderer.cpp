#include "Renderer.hpp"

#include "VulkanContext.hpp"

const uint32_t FRAME_COUNT = 3;

Renderer::Renderer() {
  auto& vulkanContext = GetCurrentVulkanContext();
  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.setCommandPool(vulkanContext.GetCommandPool())
      .setCommandBufferCount(FRAME_COUNT)
      .setLevel(vk::CommandBufferLevel::ePrimary);

  auto commandBuffers = vulkanContext.GetDevice().allocateCommandBuffers(allocInfo);

  for (uint32_t i = 0; i < FRAME_COUNT; i++) {
    auto fence = vulkanContext.GetDevice().createFence(
        vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    virtualFrames.push_back(VirtualFrame{commandBuffers[i], fence});
  }
}

void Renderer::StartFrame() {
  auto& vulkanContext = GetCurrentVulkanContext();

  auto acquiredImage = vulkanContext.GetDevice().acquireNextImageKHR(
      vulkanContext.GetSwapchain(), UINT64_MAX, vulkanContext.GetImageAvailableSemaphore());

  auto frame = GetCurrentFrame();

  vk::Result waitForFence =
      vulkanContext.GetDevice().waitForFences(frame.CommandQueueFence, false, UINT64_MAX);
  vulkanContext.GetDevice().resetFences(frame.CommandQueueFence);

  vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  frame.Commands.begin(beginInfo);

  vk::RenderPassBeginInfo renderPassInfo;
  renderPassInfo.setRenderPass(renderPass.GetRenderPass());

  frame.Commands.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
  frame.Commands.bindPipeline(vk::PipelineBindPoint::eGraphics, renderPass.GetPipeline());
}

void Renderer::EndFrame() {
  auto& vulkanContext = GetCurrentVulkanContext();
  auto frame = GetCurrentFrame();

  frame.Commands.endRenderPass();

  frame.Commands.end();

  std::array waitStages = {(vk::PipelineStageFlags)vk::PipelineStageFlagBits::eTransfer};

  vk::SubmitInfo submitInfo;
  submitInfo.setWaitSemaphores(vulkanContext.GetImageAvailableSemaphore())
      .setWaitDstStageMask(waitStages)
      .setSignalSemaphores(vulkanContext.GetRenderingFinishedSemaphore())
      .setCommandBuffers(frame.Commands);

  vulkanContext.GetGraphicsQueue().submit(submitInfo, frame.CommandQueueFence);

  vk::PresentInfoKHR presentInfo;
  presentInfo.setWaitSemaphores(vulkanContext.GetRenderingFinishedSemaphore())
      .setSwapchains(vulkanContext.GetSwapchain())
      .setImageIndices(currentFrameIndex);

  auto presentSucceeed = vulkanContext.GetPresentQueue().presentKHR(presentInfo);
  assert(presentSucceeed == vk::Result::eSuccess);

  currentFrameIndex = (currentFrameIndex + 1) % virtualFrames.size();
}
