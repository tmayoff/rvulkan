#include "Renderer.hpp"

#include <Core/Log.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "Pipeline.hpp"
#include "VulkanContext.hpp"

Renderer::Renderer(const VulkanContext& context) : context(context) {
  PipelineOptions pipelineOptions{};
  pipelineOptions.shader = Shader(context, Shader::ReadFile("rvulkan/assets/vert.spv"),
                                  Shader::ReadFile("rvulkan/assets/frag.spv"));
  pipelineOptions.bufferLayout = {BufferElement(ShaderDataType::Float3, "a_Position"),
                                  BufferElement(ShaderDataType::Float4, "a_Color")};

  vertex_buffer =
      Buffer(context, sizeof(Vertex) * QuadVertexCount,
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
             vk::BufferUsageFlagBits::eVertexBuffer);

  indexBuffer =
      Buffer(context, sizeof(uint32_t) * QuadIndices.size(),
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
             vk::BufferUsageFlagBits::eIndexBuffer);
  indexBuffer.SetData((void*)QuadIndices.data(), sizeof(uint32_t) * QuadIndices.size());

  renderPass = RenderPass(context, pipelineOptions);

  framebuffers.resize(context.GetSwapchainImageViews().size());
  for (size_t i = 0; i < context.GetSwapchainImageViews().size(); i++) {
    std::array attachments = {context.GetSwapchainImageViews().at(i)};

    auto extent = context.GetSurfaceExtent();

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.setRenderPass(renderPass.GetRenderPass());
    framebufferInfo.setAttachments(attachments);
    framebufferInfo.setWidth(extent.width);
    framebufferInfo.setHeight(extent.height);
    framebufferInfo.setLayers(1);

    framebuffers.at(i) = context.GetLogicalDevice().GetHandle().createFramebuffer(framebufferInfo);
  }

  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.setCommandPool(context.GetCommandPool())
      .setCommandBufferCount(framebuffers.size())
      .setLevel(vk::CommandBufferLevel::ePrimary);

  auto commandBuffers = context.GetLogicalDevice().GetHandle().allocateCommandBuffers(allocInfo);

  for (size_t i = 0; i < framebuffers.size(); i++) {
    auto fence = context.GetLogicalDevice().GetHandle().createFence(
        vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    virtualFrames.push_back(VirtualFrame{commandBuffers[i], fence});
  }
}

void Renderer::StartFrame() {
  auto acquiredImage = context.GetLogicalDevice().GetHandle().acquireNextImageKHR(
      context.GetSwapchain(), UINT64_MAX, context.GetImageAvailableSemaphore());
  presentImageIndex = acquiredImage.value;

  auto frame = GetCurrentFrame();

  vk::Result waitForFence = context.GetLogicalDevice().GetHandle().waitForFences(
      frame.CommandQueueFence, VK_FALSE, UINT64_MAX);
  if (waitForFence != vk::Result::eSuccess) logger::fatal("Failed to wait for fence");

  context.GetLogicalDevice().GetHandle().resetFences(frame.CommandQueueFence);

  frame.Commands.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  auto clearColors = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.2F, 0.2F, 0.2F}));

  vk::RenderPassBeginInfo renderPassInfo;
  renderPassInfo.setFramebuffer(framebuffers.at(currentFrameIndex))
      .setRenderPass(renderPass.GetRenderPass())
      .setRenderArea(vk::Rect2D({0, 0}, context.GetSurfaceExtent()))
      .setClearValues(clearColors);

  frame.Commands.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
  frame.Commands.bindPipeline(vk::PipelineBindPoint::eGraphics, renderPass.GetPipeline());
}

void Renderer::EndFrame() {
  auto frame = GetCurrentFrame();

  frame.Commands.endRenderPass();
  frame.Commands.end();

  std::array waitStages = {
      vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)};

  vk::SubmitInfo submitInfo;
  submitInfo.setWaitSemaphores(context.GetImageAvailableSemaphore())
      .setWaitDstStageMask(waitStages)
      .setSignalSemaphores(context.GetRenderingFinishedSemaphore())
      .setCommandBuffers(frame.Commands);

  context.GetLogicalDevice().GetHandle().resetFences(frame.CommandQueueFence);

  context.GetLogicalDevice().GetGraphicsQueue().submit(submitInfo, frame.CommandQueueFence);

  vk::SwapchainKHR swapchain = context.GetSwapchain();

  vk::PresentInfoKHR presentInfo;
  presentInfo.setWaitSemaphores(context.GetRenderingFinishedSemaphore())
      .setSwapchains(swapchain)
      .setImageIndices(presentImageIndex);

  auto presentSucceeed = context.GetLogicalDevice().GetPresentQueue().presentKHR(presentInfo);
  assert(presentSucceeed == vk::Result::eSuccess);

  currentFrameIndex = (currentFrameIndex + 1) % virtualFrames.size();
}

void Renderer::DrawQuad() {
  auto frame = GetCurrentFrame();

  std::vector<Vertex> vertices(QuadVertexCount);
  int i = 0;
  for (auto& v : vertices) {
    v.Position = QuadVertexPositions.at(i);
    v.Color = glm::vec4{0.1F, 0.2F, 0.8F, 1.0F};
    i++;
  }

  vertex_buffer.SetData(vertices.data(), sizeof(Vertex) * vertices.size());

  frame.Commands.bindVertexBuffers(0, vertex_buffer.GetHandle(), {0});
  frame.Commands.bindIndexBuffer(indexBuffer.GetHandle(), 0, vk::IndexType::eUint32);
  frame.Commands.drawIndexed(QuadIndices.size(), 1, 0, 0, 0);
}
