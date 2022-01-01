#include "Renderer.hpp"

#include "Pipeline.hpp"
#include "VulkanContext.hpp"

Renderer::Renderer() {
  auto& vulkanContext = GetCurrentVulkanContext();

  PipelineOptions pipelineOptions{};
  pipelineOptions.shader =
      Shader(Shader::ReadFile("assets/vert.spv"), Shader::ReadFile("assets/frag.spv"));

  renderPass = RenderPass(pipelineOptions);

  framebuffers.resize(vulkanContext.GetSwapchainImageViews().size());
  for (size_t i = 0; i < vulkanContext.GetSwapchainImageViews().size(); i++) {
    std::array attachments = {vulkanContext.GetSwapchainImageViews().at(i)};

    auto extent = vulkanContext.GetSurfaceExtent();

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.setRenderPass(renderPass.GetRenderPass());
    framebufferInfo.setAttachments(attachments);
    framebufferInfo.setWidth(extent.width);
    framebufferInfo.setHeight(extent.height);
    framebufferInfo.setLayers(1);

    framebuffers.at(i) = vulkanContext.GetDevice().createFramebuffer(framebufferInfo);
  }

  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.setCommandPool(vulkanContext.GetCommandPool())
      .setCommandBufferCount(framebuffers.size())
      .setLevel(vk::CommandBufferLevel::ePrimary);

  auto commandBuffers = vulkanContext.GetDevice().allocateCommandBuffers(allocInfo);

  for (size_t i = 0; i < framebuffers.size(); i++) {
    auto fence = vulkanContext.GetDevice().createFence(
        vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    virtualFrames.push_back(VirtualFrame{commandBuffers[i], fence});
  }
}

void Renderer::StartFrame() {
  auto& vulkanContext = GetCurrentVulkanContext();

  auto acquiredImage = vulkanContext.GetDevice().acquireNextImageKHR(
      vulkanContext.GetSwapchain(), UINT64_MAX, vulkanContext.GetImageAvailableSemaphore());
  presentImageIndex = acquiredImage.value;

  auto frame = GetCurrentFrame();

  vk::Result waitForFence =
      vulkanContext.GetDevice().waitForFences(frame.CommandQueueFence, false, UINT64_MAX);
  assert(waitForFence == vk::Result::eSuccess);
  vulkanContext.GetDevice().resetFences(frame.CommandQueueFence);

  frame.Commands.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  auto clearColors = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.2f, 0.2f, 0.2f}));

  vk::RenderPassBeginInfo renderPassInfo;
  renderPassInfo.setFramebuffer(framebuffers.at(currentFrameIndex))
      .setRenderPass(renderPass.GetRenderPass())
      .setRenderArea(vk::Rect2D({0, 0}, vulkanContext.GetSurfaceExtent()))
      .setClearValues(clearColors);

  frame.Commands.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
  frame.Commands.bindPipeline(vk::PipelineBindPoint::eGraphics, renderPass.GetPipeline());

  renderPass.Render({frame.Commands});
}

void Renderer::EndFrame() {
  auto& vulkanContext = GetCurrentVulkanContext();
  auto frame = GetCurrentFrame();

  frame.Commands.endRenderPass();
  frame.Commands.end();

  std::array waitStages = {
      (vk::PipelineStageFlags)vk::PipelineStageFlagBits::eColorAttachmentOutput};

  vk::SubmitInfo submitInfo;
  submitInfo.setWaitSemaphores(vulkanContext.GetImageAvailableSemaphore())
      .setWaitDstStageMask(waitStages)
      .setSignalSemaphores(vulkanContext.GetRenderingFinishedSemaphore())
      .setCommandBuffers(frame.Commands);

  vulkanContext.GetDevice().resetFences(frame.CommandQueueFence);

  vulkanContext.GetGraphicsQueue().submit(submitInfo, frame.CommandQueueFence);

  vk::PresentInfoKHR presentInfo;
  presentInfo.setWaitSemaphores(vulkanContext.GetRenderingFinishedSemaphore())
      .setSwapchains(vulkanContext.GetSwapchain())
      .setImageIndices(presentImageIndex);

  auto presentSucceeed = vulkanContext.GetPresentQueue().presentKHR(presentInfo);
  assert(presentSucceeed == vk::Result::eSuccess);

  currentFrameIndex = (currentFrameIndex + 1) % virtualFrames.size();
}
