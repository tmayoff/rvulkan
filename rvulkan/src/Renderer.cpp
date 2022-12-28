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
    : context(context) {
  surface_extent = vk::Extent2D(resolution.first, resolution.second);

  // Uniform buffer
  uniform_buffer = std::make_shared<Buffer>(context, sizeof(uniform_buffer_data),
                                            VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                                            vk::BufferUsageFlagBits::eUniformBuffer);

  CreateRenderPass();

  swapchain = Swapchain(context, renderPass, surface_extent);

  CreateCommandBuffers();
  CreateSyncObjects();
}

void Renderer::StartFrame(const glm::mat4& view_projection) {
  uniform_buffer_data.view_projection = view_projection;
  uniform_buffer->SetData((void*)&uniform_buffer_data, sizeof(uniform_buffer_data));

  const auto device = context->GetLogicalDevice().GetHandle();

  vk::Result wait_for_fence =
      device.waitForFences(in_flight_fences[current_frame_index], VK_FALSE, UINT64_MAX);
  if (wait_for_fence != vk::Result::eSuccess) logger::fatal("Failed to wait for fence");
  device.resetFences(in_flight_fences[current_frame_index]);

  const auto acquired_image_index = device.acquireNextImageKHR(
      swapchain.GetHandle(), UINT64_MAX, image_available_semaphores[current_frame_index]);
  if (acquired_image_index.result == vk::Result::eErrorOutOfDateKHR) {
    swapchain.RecreateSwapchain(surface_extent);
    return;
  }

  present_image_index = acquired_image_index.value;

  command_buffers[current_frame_index].reset();
  command_buffers[current_frame_index].begin(
      vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

  auto clear_colours = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1F, 0.1F, 0.1F}));
  vk::RenderPassBeginInfo render_pass_info(renderPass->GetHandle(),
                                           swapchain.GetFramebuffers().at(current_frame_index),
                                           vk::Rect2D({0, 0}, surface_extent), clear_colours);

  command_buffers[current_frame_index].beginRenderPass(render_pass_info,
                                                       vk::SubpassContents::eInline);
  command_buffers[current_frame_index].bindPipeline(vk::PipelineBindPoint::eGraphics,
                                                    renderPass->GetPipeline()->GetHandle());

  command_buffers[current_frame_index].setViewport(
      0, vk::Viewport(0, 0, surface_extent.width, surface_extent.height));
  command_buffers[current_frame_index].setScissor(0, vk::Rect2D({0, 0}, surface_extent));

  command_buffers[current_frame_index].bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, renderPass->GetPipeline()->GetLayout(), 0,
      renderPass->GetPipeline()->GetDescriptorSets(), nullptr);
}

void Renderer::EndFrame() {
  const auto device = context->GetLogicalDevice();

  command_buffers[current_frame_index].endRenderPass();
  command_buffers[current_frame_index].end();

  std::array wait_stages = {
      vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput)};

  auto wait_semaphore = image_available_semaphores[current_frame_index];
  auto signal_semaphore = render_finished_semaphores[current_frame_index];

  vk::SubmitInfo submit_info(wait_semaphore, wait_stages, command_buffers[current_frame_index],
                             signal_semaphore);

  device.GetPresentQueue().submit(submit_info, in_flight_fences[current_frame_index]);

  vk::PresentInfoKHR present_info(signal_semaphore, swapchain.GetHandle(), present_image_index);

  auto present_result = device.GetPresentQueue().presentKHR(present_info);
  if (present_result == vk::Result::eErrorOutOfDateKHR ||
      present_result == vk::Result::eSuboptimalKHR || view_resized) {
    view_resized = false;
    swapchain.RecreateSwapchain(surface_extent);
  }

  current_frame_index = (current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::ResizeViewport(resolution_t size) {
  surface_extent = vk::Extent2D(size.first, size.second);
  view_resized = true;
}

void Renderer::DrawMesh(const Component::MeshRenderer& mesh_renderer) {
  command_buffers[current_frame_index].bindVertexBuffers(
      0, mesh_renderer.GetMesh().GetVertexBuffer()->GetHandle(), {0});
  command_buffers[current_frame_index].bindIndexBuffer(
      mesh_renderer.GetMesh().GetIndexBuffer()->GetHandle(), 0, vk::IndexType::eUint32);
  command_buffers[current_frame_index].drawIndexed(mesh_renderer.GetMesh().GetIndices().size(), 1,
                                                   0, 0, 0);
}

void Renderer::CreateRenderPass() {
  PipelineOptions pipeline_options{};
  pipeline_options.shader = Shader(context, Shader::ReadFile("rvulkan/assets/shaders/vert.spv"),
                                   Shader::ReadFile("rvulkan/assets/shaders/frag.spv"));
  pipeline_options.surface_extent = surface_extent;
  pipeline_options.bufferLayout = Vertex::GetLayout();
  pipeline_options.uniform_buffer_layouts = {Vertex::GetUniformLayout()};

  renderPass = std::make_shared<RenderPass>(context, pipeline_options);

  // Update Descriptor Sets
  vk::DescriptorBufferInfo buffer_info(uniform_buffer->GetHandle(), 0, sizeof(uniform_buffer_data));
  vk::WriteDescriptorSet write_descriptor_set(renderPass->GetPipeline()->GetDescriptorSets()[0], 0,
                                              0, 1, vk::DescriptorType::eUniformBuffer, nullptr,
                                              &buffer_info);
  context->GetLogicalDevice().GetHandle().updateDescriptorSets(write_descriptor_set, nullptr);
}

void Renderer::CreateCommandBuffers() {
  command_buffers.reserve(MAX_FRAMES_IN_FLIGHT);

  vk::CommandBufferAllocateInfo alloc_info(context->GetCommandPool(),
                                           vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
  command_buffers = context->GetLogicalDevice().GetHandle().allocateCommandBuffers(alloc_info);
}

void Renderer::CreateSyncObjects() {
  auto device = context->GetLogicalDevice().GetHandle();

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
