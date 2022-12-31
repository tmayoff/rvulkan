#include "Renderer.hpp"

#include <vulkan/vulkan_core.h>

#include <Core/Log.hpp>
#include <debug/profiler.hpp>
#include <exception>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Buffer.hpp"
#include "Core/types.hpp"
#include "Pipeline.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/render_context.hpp"
#include "renderer/swapchain.hpp"

Renderer::Renderer(const std::shared_ptr<VulkanContext>& context)
    : vulkan_context(context), render_context(context) {
  // Uniform buffer
  uniform_buffer = std::make_shared<Buffer>(context, sizeof(uniform_buffer_data),
                                            VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                                            vk::BufferUsageFlagBits::eUniformBuffer);

  // Update Descriptor Sets
  vk::DescriptorBufferInfo buffer_info(uniform_buffer->GetHandle(), 0, sizeof(uniform_buffer_data));
  vk::WriteDescriptorSet write_descriptor_set(
      render_context.GetRenderPass()->GetPipeline()->GetDescriptorSets()[0], 0, 0, 1,
      vk::DescriptorType::eUniformBuffer, nullptr, &buffer_info);
  context->GetLogicalDevice().GetHandle().updateDescriptorSets(write_descriptor_set, nullptr);
}

void Renderer::BeginFrame(const glm::mat4& view_projection) {
  ZoneScoped;  // NOLINT

  uniform_buffer_data.view_projection = view_projection;
  uniform_buffer->SetData((void*)&uniform_buffer_data, sizeof(uniform_buffer_data));

  render_context.BeginFrame();
}

void Renderer::EndFrame() {
  ZoneScoped;  // NOLINT

  render_context.EndFrame();
}

void Renderer::DrawMesh(const Component::MeshRenderer& mesh_renderer) {
  ZoneScoped;  // NOLINT

  render_context.BindVertexBuffer(0, mesh_renderer.GetMesh().GetVertexBuffer()->GetHandle(), {0});
  render_context.BindIndexBuffer(mesh_renderer.GetMesh().GetIndexBuffer()->GetHandle());
  render_context.DrawIndexed(mesh_renderer.GetMesh().GetIndices().size());
}
