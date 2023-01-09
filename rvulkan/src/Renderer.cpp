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
#include "vulkan/LogicalDevice.hpp"

Renderer::Renderer(std::shared_ptr<VulkanContext> vulkan_context_)
    : vulkan_context(std::move(vulkan_context_)) {
  // Setup render passes
  PipelineOptions options{};

  auto shader =
      std::make_unique<Shader>(vulkan_context->GetLogicalDevice()->GetHandle(),
                               ShaderCode{Shader::ReadFile("rvulkan/assets/shaders/vert.spv"),
                                          Shader::ReadFile("rvulkan/assets/shaders/frag.spv")});
  options.surface_extent = surface_extent;
  options.bufferLayout = Vertex::GetLayout();
  options.uniform_buffer_layouts = {Vertex::GetUniformLayout()};
  present_render_pass = std::make_shared<RenderPass>(vulkan_context, std::move(shader), options);

  render_context = std::make_shared<RenderContext>(vulkan_context, present_render_pass);
}

void Renderer::BeginFrame() {
  ZoneScopedN(__PRETTY_FUNCTION__);  // NOLINT

  render_context->PrepareFrame();

  {
    // Record Command Buffers
    ZoneScopedN("Record Command Buffers");  // NOLINT

    auto cmd_buffer = render_context->GetCurrentCommandBuffer();

    cmd_buffer.reset();
    cmd_buffer.begin(vk::CommandBufferBeginInfo());

    auto clear_colours =
        vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.1F, 0.1F, 0.1F}));
    vk::RenderPassBeginInfo render_pass_info(present_render_pass->GetHandle(),
                                             render_context->GetCurrentFrameBuffer(),
                                             vk::Rect2D({0, 0}, surface_extent), clear_colours);

    cmd_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
    cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                            present_render_pass->GetPipeline()->GetHandle());

    cmd_buffer.setViewport(0, vk::Viewport(0, 0, surface_extent.width, surface_extent.height));
    cmd_buffer.setScissor(0, vk::Rect2D({0, 0}, surface_extent));
  }
}

void Renderer::EndFrame() {
  ZoneScopedN(__PRETTY_FUNCTION__);  // NOLINT

  render_context->GetCurrentCommandBuffer().endRenderPass();
  render_context->GetCurrentCommandBuffer().end();

  render_context->PresentFrame();
}

void Renderer::DrawMesh(const std::shared_ptr<RenderContext>& render_context,
                        const Component::MeshRenderer& mesh_renderer, const glm::mat4& transform) {
  ZoneScopedN(__PRETTY_FUNCTION__);  // NOLINT

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
  render_context->BindVertexBuffer(0, mesh_renderer.GetMesh().GetVertexBuffer()->GetHandle(), {0});
  render_context->BindIndexBuffer(mesh_renderer.GetMesh().GetIndexBuffer()->GetHandle());
  render_context->DrawIndexed(mesh_renderer.GetMesh().GetIndices().size());
}
