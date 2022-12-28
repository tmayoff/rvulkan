#include "Renderer.hpp"

#include <vulkan/vulkan_core.h>

#include <Core/Log.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Buffer.hpp"
#include "Core/types.hpp"
#include "Pipeline.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/swapchain.hpp"

Renderer::Renderer(const std::shared_ptr<VulkanContext>& context, const resolution_t& resolution)
    : context(context), swapchain(context, vk::Extent2D(resolution.first, resolution.second)) {
  surface_extent = vk::Extent2D(resolution.first, resolution.second);

  // Uniform buffer
  uniform_buffer = std::make_shared<Buffer>(context, sizeof(uniform_buffer_data),
                                            VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                                            vk::BufferUsageFlagBits::eUniformBuffer);

  CreateRenderPass();
  CreateFramebuffers();
  CreateVirtualFrames();
}

void Renderer::StartFrame(const glm::mat4& view_projection) {
  uniform_buffer_data.view_projection = view_projection;
  uniform_buffer->SetData((void*)&uniform_buffer_data, sizeof(uniform_buffer_data));

  const auto device = context->GetLogicalDevice().GetHandle();
  const auto frame = GetCurrentFrame();

  vk::Result wait_for_fence = device.waitForFences(frame->GetInFlight(), VK_FALSE, UINT64_MAX);
  if (wait_for_fence != vk::Result::eSuccess) logger::fatal("Failed to wait for fence");
  device.resetFences(frame->GetInFlight());

  const auto acquired_image = context->GetLogicalDevice().GetHandle().acquireNextImageKHR(
      swapchain.GetHandle(), UINT64_MAX, frame->GetImageAvailable());
  if (acquired_image.result == vk::Result::eErrorOutOfDateKHR) {
    // swapchain.RecreateSwapchain(surface_extent);
    // return;
  }

  present_image_index = acquired_image.value;

  frame->GetCommandBuffer().begin(
      vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  auto clear_colours = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1F, 0.1F, 0.1F}));
  vk::RenderPassBeginInfo render_pass_info(renderPass->GetHandle(),
                                           framebuffers.at(current_frame_index),
                                           vk::Rect2D({0, 0}, surface_extent), clear_colours);

  frame->GetCommandBuffer().setScissor(0, vk::Rect2D({0, 0}, surface_extent));
  frame->GetCommandBuffer().setViewport(
      0, vk::Viewport(0, 0, surface_extent.width, surface_extent.height));

  frame->GetCommandBuffer().beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
  frame->GetCommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics,
                                         renderPass->GetPipeline()->GetHandle());
  frame->GetCommandBuffer().bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, renderPass->GetPipeline()->GetLayout(), 0,
      renderPass->GetPipeline()->GetDescriptorSets(), nullptr);
}

void Renderer::EndFrame() {
  auto frame = GetCurrentFrame();

  frame->GetCommandBuffer().endRenderPass();
  frame->GetCommandBuffer().end();

  std::array waitStages = {
      vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)};

  vk::SubmitInfo submitInfo;
  submitInfo.setWaitSemaphores(frame->GetImageAvailable())
      .setWaitDstStageMask(waitStages)
      .setSignalSemaphores(frame->GetRenderFinished())
      .setCommandBuffers(frame->GetCommandBuffer());

  context->GetLogicalDevice().GetHandle().resetFences(frame->GetInFlight());
  context->GetLogicalDevice().GetGraphicsQueue().submit(submitInfo, frame->GetInFlight());

  vk::PresentInfoKHR presentInfo;
  presentInfo.setWaitSemaphores(frame->GetRenderFinished())
      .setSwapchains(swapchain.GetHandle())
      .setImageIndices(present_image_index);

  auto present_result = context->GetLogicalDevice().GetPresentQueue().presentKHR(presentInfo);
  if (present_result == vk::Result::eErrorOutOfDateKHR ||
      present_result == vk::Result::eSuboptimalKHR || view_resized) {
    view_resized = false;
    // swapchain.RecreateSwapchain(surface_extent);
  }
}

void Renderer::ResizeViewport(resolution_t size) {
  surface_extent = vk::Extent2D(size.first, size.second);
  view_resized = true;
}

void Renderer::DrawMesh(const Component::MeshRenderer& mesh_renderer) {
  const auto& frame = GetCurrentFrame();

  frame->GetCommandBuffer().bindVertexBuffers(
      0, mesh_renderer.GetMesh().GetVertexBuffer()->GetHandle(), {0});
  frame->GetCommandBuffer().bindIndexBuffer(mesh_renderer.GetMesh().GetIndexBuffer()->GetHandle(),
                                            0, vk::IndexType::eUint32);
  frame->GetCommandBuffer().drawIndexed(mesh_renderer.GetMesh().GetIndices().size(), 1, 0, 0, 0);
}

void Renderer::CreateRenderPass() {
  PipelineOptions pipelineOptions{};
  pipelineOptions.shader = Shader(context, Shader::ReadFile("rvulkan/assets/shaders/vert.spv"),
                                  Shader::ReadFile("rvulkan/assets/shaders/frag.spv"));
  pipelineOptions.surface_extent = surface_extent;
  pipelineOptions.bufferLayout = Vertex::GetLayout();
  pipelineOptions.uniform_buffer_layouts = {Vertex::GetUniformLayout()};

  renderPass = std::make_shared<RenderPass>(context, pipelineOptions);

  // Update Descriptor Sets
  vk::DescriptorBufferInfo buffer_info(uniform_buffer->GetHandle(), 0, sizeof(uniform_buffer_data));
  vk::WriteDescriptorSet write_descriptor_set(renderPass->GetPipeline()->GetDescriptorSets()[0], 0,
                                              0, 1, vk::DescriptorType::eUniformBuffer, nullptr,
                                              &buffer_info);
  context->GetLogicalDevice().GetHandle().updateDescriptorSets(write_descriptor_set, nullptr);
}

void Renderer::CreateVirtualFrames() {
  vk::CommandBufferAllocateInfo buffer_alloc(context->GetCommandPool(),
                                             vk::CommandBufferLevel::ePrimary, framebuffers.size());

  auto command_buffers =
      context->GetLogicalDevice().GetHandle().allocateCommandBuffers(buffer_alloc);

  for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    virtual_frames.push_back(std::make_shared<VirtualFrame>(context, command_buffers[i]));
  }
}

void Renderer::CreateFramebuffers() {
  framebuffers.resize(swapchain.GetImageViews().size());

  for (size_t i = 0; i < swapchain.GetImageViews().size(); i++) {
    std::array attachments = {swapchain.GetImageViews().at(i)};

    vk::FramebufferCreateInfo framebufferInfo({}, renderPass->GetHandle(), attachments,
                                              surface_extent.width, surface_extent.height, 1);
    framebuffers.at(i) = context->GetLogicalDevice().GetHandle().createFramebuffer(framebufferInfo);
  }
}
