#include "Renderer.hpp"

#include <Core/Log.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "Pipeline.hpp"
#include "VulkanContext.hpp"

Renderer::Renderer(const VulkanContext& context) {
  PipelineOptions pipelineOptions{};
  pipelineOptions.shader = Shader(Shader::ReadFile("rvulkan/assets/vert.spv"),
                                  Shader::ReadFile("rvulkan/assets/frag.spv"));
  pipelineOptions.bufferLayout = {BufferElement(ShaderDataType::Float3, "a_Position"),
                                  BufferElement(ShaderDataType::Float4, "a_Color")};

  vertexBuffer = Buffer(sizeof(Vertex) * QuadVertexCount, VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                        vk::BufferUsageFlagBits::eVertexBuffer);

  // indexBuffer =
  //     Buffer(sizeof(uint32_t) * QuadIndices.size(), VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
  //            vk::BufferUsageFlagBits::eIndexBuffer);
  // indexBuffer.SetData((void*)QuadIndices.data(), sizeof(uint32_t) * QuadIndices.size());

  // renderPass = RenderPass(pipelineOptions);

  // framebuffers.resize(context.GetSwapchainImageViews().size());
  // for (size_t i = 0; i < context.GetSwapchainImageViews().size(); i++) {
  //   std::array attachments = {context.GetSwapchainImageViews().at(i)};

  //   auto extent = context.GetSurfaceExtent();

  //   vk::FramebufferCreateInfo framebufferInfo{};
  //   framebufferInfo.setRenderPass(renderPass.GetRenderPass());
  //   framebufferInfo.setAttachments(attachments);
  //   framebufferInfo.setWidth(extent.width);
  //   framebufferInfo.setHeight(extent.height);
  //   framebufferInfo.setLayers(1);

  //   framebuffers.at(i) = context.GetDevice().createFramebuffer(framebufferInfo);
  // }

  // vk::CommandBufferAllocateInfo allocInfo;
  // allocInfo.setCommandPool(context.GetCommandPool())
  //     .setCommandBufferCount(framebuffers.size())
  //     .setLevel(vk::CommandBufferLevel::ePrimary);

  // auto commandBuffers = context.GetDevice().allocateCommandBuffers(allocInfo);

  // for (size_t i = 0; i < framebuffers.size(); i++) {
  //   auto fence =
  //       context.GetDevice().createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
  //   virtualFrames.push_back(VirtualFrame{commandBuffers[i], fence});
  // }
}

void Renderer::StartFrame() {
  // auto& vulkanContext = GetCurrentVulkanContext();

  // auto acquiredImage = vulkanContext.GetDevice().acquireNextImageKHR(
  //     vulkanContext.GetSwapchain().swapchain, UINT64_MAX,
  //     vulkanContext.GetImageAvailableSemaphore());
  // presentImageIndex = acquiredImage.value;

  // auto frame = GetCurrentFrame();

  // vk::Result waitForFence =
  //     vulkanContext.GetDevice().waitForFences(frame.CommandQueueFence, VK_FALSE, UINT64_MAX);
  // if (waitForFence != vk::Result::eSuccess) logger::fatal("Failed to wait for fence");

  // vulkanContext.GetDevice().resetFences(frame.CommandQueueFence);

  // frame.Commands.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  // auto clearColors = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.2f, 0.2f, 0.2f}));

  // vk::RenderPassBeginInfo renderPassInfo;
  // renderPassInfo.setFramebuffer(framebuffers.at(currentFrameIndex))
  //     .setRenderPass(renderPass.GetRenderPass())
  //     .setRenderArea(vk::Rect2D({0, 0}, vulkanContext.GetSurfaceExtent()))
  //     .setClearValues(clearColors);

  // frame.Commands.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
  // frame.Commands.bindPipeline(vk::PipelineBindPoint::eGraphics, renderPass.GetPipeline());
}

void Renderer::EndFrame() {
  // auto& vulkanContext = GetCurrentVulkanContext();
  // auto frame = GetCurrentFrame();

  // frame.Commands.endRenderPass();
  // frame.Commands.end();

  // std::array waitStages = {
  //     vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)};

  // vk::SubmitInfo submitInfo;
  // submitInfo.setWaitSemaphores(vulkanContext.GetImageAvailableSemaphore())
  //     .setWaitDstStageMask(waitStages)
  //     .setSignalSemaphores(vulkanContext.GetRenderingFinishedSemaphore())
  //     .setCommandBuffers(frame.Commands);

  // vulkanContext.GetDevice().resetFences(frame.CommandQueueFence);

  // vulkanContext.GetGraphicsQueue().submit(submitInfo, frame.CommandQueueFence);

  // vk::SwapchainKHR swapchain = vulkanContext.GetSwapchain().swapchain;

  // vk::PresentInfoKHR presentInfo;
  // presentInfo.setWaitSemaphores(vulkanContext.GetRenderingFinishedSemaphore())
  //     .setSwapchains(swapchain)
  //     .setImageIndices(presentImageIndex);

  // auto presentSucceeed = vulkanContext.GetPresentQueue().presentKHR(presentInfo);
  // assert(presentSucceeed == vk::Result::eSuccess);

  // currentFrameIndex = (currentFrameIndex + 1) % virtualFrames.size();
}

void Renderer::DrawQuad() {
  // auto frame = GetCurrentFrame();

  // std::vector<Vertex> vertices(QuadVertexCount);
  // int i = 0;
  // for (auto& v : vertices) {
  //   v.Position = QuadVertexPositions.at(i);
  //   v.Color = glm::vec4{0.1F, 0.2F, 0.8F, 1.0F};
  //   i++;
  // }

  // vertexBuffer.SetData(vertices.data(), sizeof(Vertex) * vertices.size());

  // frame.Commands.bindVertexBuffers(0, vertexBuffer.GetHandle(), {0});
  // frame.Commands.bindIndexBuffer(indexBuffer.GetHandle(), 0, vk::IndexType::eUint32);
  // frame.Commands.drawIndexed(QuadIndices.size(), 1, 0, 0, 0);
}
