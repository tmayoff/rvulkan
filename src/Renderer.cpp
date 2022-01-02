#include "Renderer.hpp"

#include "Pipeline.hpp"
#include "VulkanContext.hpp"

Renderer::Renderer() {
  auto& vulkanContext = GetCurrentVulkanContext();

  PipelineOptions pipelineOptions{};
  pipelineOptions.shader =
      Shader(Shader::ReadFile("assets/vert.spv"), Shader::ReadFile("assets/frag.spv"));
  pipelineOptions.bufferLayout = {BufferElement(ShaderDataType::Float3, "a_Position"),
                                  BufferElement(ShaderDataType::Float4, "a_Color")};

  vertexBuffer =
      Buffer(sizeof(Vertex) * QuadVertexCount, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_TO_CPU,
             vk::BufferUsageFlagBits::eVertexBuffer);

  indexBuffer =
      Buffer(sizeof(uint32_t) * QuadIndices.size(), VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_TO_CPU,
             vk::BufferUsageFlagBits::eIndexBuffer);
  indexBuffer.SetData((void*)QuadIndices.data(), sizeof(uint32_t) * QuadIndices.size());

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

void Renderer::DrawQuad() {
  auto frame = GetCurrentFrame();

  std::vector<Vertex> vertices(QuadVertexCount);
  int i = 0;
  for (auto& v : vertices) {
    v.Position = QuadVertexPositions.at(i);
    v.Color = glm::vec4{0.1f, 0.2f, 0.8f, 1.0f};
    i++;
  }

  vertexBuffer.SetData(vertices.data(), sizeof(Vertex) * vertices.size());

  frame.Commands.bindVertexBuffers(0, vertexBuffer.GetHandle(), {0});
  frame.Commands.bindIndexBuffer(indexBuffer.GetHandle(), 0, vk::IndexType::eUint32);
  frame.Commands.drawIndexed(QuadIndices.size(), 1, 0, 0, 0);
}
