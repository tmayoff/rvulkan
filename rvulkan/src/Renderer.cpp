#include "Renderer.hpp"

#include <vulkan/vulkan_core.h>

#include <debug/profiler.hpp>
#include <rvulkan/core/log.hpp>
#include <rvulkan/core/types.hpp>
#include <rvulkan/renderer/mesh.hpp>
#include <rvulkan/scene/components/mesh_renderer.hpp>
#include <vulkan/vulkan.hpp>

#include "Buffer.hpp"
#include "Pipeline.hpp"
#include "renderer/render_context.hpp"
#include "renderer/swapchain.hpp"

Renderer::Renderer(const std::shared_ptr<VulkanContext>& context)
    : vulkan_context(context), render_context(context) {}

void Renderer::BeginFrame() {
  ZoneScoped;  // NOLINT

  render_context.BeginFrame();
}

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
