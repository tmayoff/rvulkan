#include "Renderer.hpp"

#include <vulkan/vulkan_core.h>

#include <Core/Log.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Buffer.hpp"
#include "Pipeline.hpp"
#include "renderer/Mesh.hpp"

Renderer::Renderer(const std::shared_ptr<VulkanContext>& context) : context(context) {
  // Uniform buffer
  uniform_buffer = std::make_shared<Buffer>(context, sizeof(uniform_buffer_data),
                                            VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                                            vk::BufferUsageFlagBits::eUniformBuffer);

  PipelineOptions pipelineOptions{};
  pipelineOptions.shader = Shader(context, Shader::ReadFile("rvulkan/assets/shaders/vert.spv"),
                                  Shader::ReadFile("rvulkan/assets/shaders/frag.spv"));

  pipelineOptions.bufferLayout = Vertex::GetLayout();
  pipelineOptions.uniform_buffer_layouts = {Vertex::GetUniformLayout()};

  renderPass = RenderPass(context, pipelineOptions);

  // Update Descriptor Sets
  vk::DescriptorBufferInfo buffer_info(uniform_buffer->GetHandle(), 0, sizeof(uniform_buffer_data));
  vk::WriteDescriptorSet write_descriptor_set(renderPass.GetPipeline().GetDescriptorSets()[0], 0, 0,
                                              1, vk::DescriptorType::eUniformBuffer, nullptr,
                                              &buffer_info);
  context->GetLogicalDevice().GetHandle().updateDescriptorSets(write_descriptor_set, nullptr);

  framebuffers.resize(context->GetSwapchainImageViews().size());
  for (size_t i = 0; i < context->GetSwapchainImageViews().size(); i++) {
    std::array attachments = {context->GetSwapchainImageViews().at(i)};

    auto extent = context->GetSurfaceExtent();

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.setRenderPass(renderPass.GetRenderPass());
    framebufferInfo.setAttachments(attachments);
    framebufferInfo.setWidth(extent.width);
    framebufferInfo.setHeight(extent.height);
    framebufferInfo.setLayers(1);

    framebuffers.at(i) = context->GetLogicalDevice().GetHandle().createFramebuffer(framebufferInfo);
  }

  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.setCommandPool(context->GetCommandPool())
      .setCommandBufferCount(framebuffers.size())
      .setLevel(vk::CommandBufferLevel::ePrimary);

  auto commandBuffers = context->GetLogicalDevice().GetHandle().allocateCommandBuffers(allocInfo);

  for (size_t i = 0; i < framebuffers.size(); i++) {
    auto fence = context->GetLogicalDevice().GetHandle().createFence(
        vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    virtualFrames.push_back(VirtualFrame{commandBuffers[i], fence});
  }
}

void Renderer::StartFrame(const glm::mat4& view_projection) {
  uniform_buffer_data.view_projection = view_projection;
  uniform_buffer->SetData((void*)&uniform_buffer_data, sizeof(uniform_buffer_data));

  auto acquiredImage = context->GetLogicalDevice().GetHandle().acquireNextImageKHR(
      context->GetSwapchain(), UINT64_MAX, context->GetImageAvailableSemaphore());
  presentImageIndex = acquiredImage.value;

  auto frame = GetCurrentFrame();

  vk::Result waitForFence = context->GetLogicalDevice().GetHandle().waitForFences(
      frame.CommandQueueFence, VK_FALSE, UINT64_MAX);
  if (waitForFence != vk::Result::eSuccess) logger::fatal("Failed to wait for fence");

  context->GetLogicalDevice().GetHandle().resetFences(frame.CommandQueueFence);

  frame.Commands.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  auto clearColors = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1F, 0.1F, 0.1F}));

  vk::RenderPassBeginInfo renderPassInfo;
  renderPassInfo.setFramebuffer(framebuffers.at(currentFrameIndex))
      .setRenderPass(renderPass.GetRenderPass())
      .setRenderArea(vk::Rect2D({0, 0}, context->GetSurfaceExtent()))
      .setClearValues(clearColors);

  frame.Commands.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
  frame.Commands.bindPipeline(vk::PipelineBindPoint::eGraphics,
                              renderPass.GetPipeline().GetHandle());
  frame.Commands.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                    renderPass.GetPipeline().GetLayout(), 0,
                                    renderPass.GetPipeline().GetDescriptorSets(), nullptr);
}

void Renderer::EndFrame() {
  auto frame = GetCurrentFrame();

  frame.Commands.endRenderPass();
  frame.Commands.end();

  std::array waitStages = {
      vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)};

  vk::SubmitInfo submitInfo;
  submitInfo.setWaitSemaphores(context->GetImageAvailableSemaphore())
      .setWaitDstStageMask(waitStages)
      .setSignalSemaphores(context->GetRenderingFinishedSemaphore())
      .setCommandBuffers(frame.Commands);

  context->GetLogicalDevice().GetHandle().resetFences(frame.CommandQueueFence);

  context->GetLogicalDevice().GetGraphicsQueue().submit(submitInfo, frame.CommandQueueFence);

  vk::SwapchainKHR swapchain = context->GetSwapchain();

  vk::PresentInfoKHR presentInfo;
  presentInfo.setWaitSemaphores(context->GetRenderingFinishedSemaphore())
      .setSwapchains(swapchain)
      .setImageIndices(presentImageIndex);

  auto present_result = context->GetLogicalDevice().GetPresentQueue().presentKHR(presentInfo);
  if (present_result == vk::Result::eErrorOutOfDateKHR ||
      present_result == vk::Result::eSuboptimalKHR || view_resized) {
    // context->RecreateSwapchain(view_resized->first, view_resized->second);

    // view_resized.reset();
  }

  currentFrameIndex = (currentFrameIndex + 1) % virtualFrames.size();
}

void Renderer::ResizeViewport(std::pair<float, float> size) { view_resized = size; }

void Renderer::DrawMesh(const Component::MeshRenderer& mesh_renderer) {
  const auto& frame = GetCurrentFrame();

  frame.Commands.bindVertexBuffers(0, mesh_renderer.GetMesh().GetVertexBuffer()->GetHandle(), {0});
  frame.Commands.bindIndexBuffer(mesh_renderer.GetMesh().GetIndexBuffer()->GetHandle(), 0,
                                 vk::IndexType::eUint32);
  frame.Commands.drawIndexed(mesh_renderer.GetMesh().GetIndices().size(), 1, 0, 0, 0);
}
