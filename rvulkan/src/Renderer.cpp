#include "Renderer.hpp"

#include <vulkan/vulkan_core.h>

#include <core/log.hpp>
#include <debug/profiler.hpp>
#include <exception>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Buffer.hpp"
#include "Pipeline.hpp"
#include "core/types.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/render_context.hpp"
#include "renderer/swapchain.hpp"

Renderer::Renderer(const std::shared_ptr<VulkanContext>& context)
    : vulkan_context(context), render_context(context) {
  // Uniform buffer
  // uniform_buffer = std::make_shared<Buffer>(context, sizeof(uniform_buffer_data),
  //                                           VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
  //                                           vk::BufferUsageFlagBits::eUniformBuffer);

  // Update Descriptor Sets
  // vk::DescriptorBufferInfo buffer_info(uniform_buffer->GetHandle(), 0,
  // sizeof(uniform_buffer_data)); vk::WriteDescriptorSet write_descriptor_set(
  //     render_context.GetRenderPass()->GetPipeline()->GetDescriptorSets()[0], 0, 0, 1,
  //     vk::DescriptorType::eUniformBuffer, nullptr, &buffer_info);
  // context->GetLogicalDevice().GetHandle().updateDescriptorSets(write_descriptor_set, nullptr);
}

void Renderer::BeginFrame() {
  ZoneScoped;  // NOLINT

  render_context.BeginFrame();
}

void Renderer::BeginScene(const glm::mat4& view_projection) {
  // uniform_buffer_data.view_projection = view_projection;
  // uniform_buffer->SetData((void*)&uniform_buffer_data, sizeof(uniform_buffer_data));
}

void Renderer::EndScene() {}

void Renderer::EndFrame() {
  ZoneScoped;  // NOLINT

  render_context.EndFrame();
}

void Renderer::DrawMesh(const RenderContext& render_context,
                        const Component::MeshRenderer& mesh_renderer, const glm::mat4& transform) {
  ZoneScoped;  // NOLINT

  const auto& mesh = mesh_renderer.GetMesh();
  std::vector<Vertex> vertices = mesh.GetVertices();
  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i].Position = transform * mesh.GetVertices()[i].Position;
    vertices[i].Color = mesh.GetVertices()[i].Color;
    vertices[i].Normal = mesh.GetVertices()[i].Normal;
    vertices[i].TexCoord = mesh.GetVertices()[i].TexCoord;
  }

  mesh_renderer.GetMesh().GetVertexBuffer()->SetData(vertices.data(),
                                                     vertices.size() * sizeof(Vertex));
  render_context.BindVertexBuffer(0, mesh_renderer.GetMesh().GetVertexBuffer()->GetHandle(), {0});
  render_context.BindIndexBuffer(mesh_renderer.GetMesh().GetIndexBuffer()->GetHandle());
  render_context.DrawIndexed(mesh_renderer.GetMesh().GetIndices().size());
}
