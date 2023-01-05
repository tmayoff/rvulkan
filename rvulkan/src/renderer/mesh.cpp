#include "rvulkan/renderer/mesh.hpp"

#include <Buffer.hpp>
#include <cstdint>
#include <rvulkan/core/memory.hpp>
#include <vector>
#include <vulkan/vulkan_enums.hpp>

#include "glm/fwd.hpp"

Mesh Mesh::CreateQuadMesh(const std::shared_ptr<VulkanContext>& context) {
  Mesh mesh;

  glm::vec4 color{1.0F};
  glm::vec3 normal{0.0F, 0.0F, 1.0F};

  mesh.vertices = std::vector<Vertex>(4);

  mesh.vertices[0] = {{-0.5F, -0.5F, 0.0F, 1.0F}, color, {1.0F, 0.0F}, normal};
  mesh.vertices[1] = {{0.5F, -0.5F, 0.0F, 1.0F}, color, {1.0F, 1.0F}, normal};
  mesh.vertices[2] = {{0.5F, 0.5F, 0.0F, 1.0F}, color, {1.0F, 0.0F}, normal};
  mesh.vertices[3] = {{-0.5F, 0.5F, 0.0F, 1.0F}, color, {1.0F, 0.0F}, normal};

  mesh.indices = std::vector<uint32_t>(6);

  mesh.indices[0] = 0;
  mesh.indices[1] = 1;
  mesh.indices[2] = 2;
  mesh.indices[3] = 2;
  mesh.indices[4] = 3;
  mesh.indices[5] = 0;

  mesh.vertex_buffer = std::make_shared<Buffer>(context, sizeof(Vertex) * mesh.vertices.size(),
                                                VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                                                vk::BufferUsageFlagBits::eVertexBuffer);
  mesh.vertex_buffer->SetData(mesh.vertices.data(), sizeof(Vertex) * mesh.vertices.size());

  mesh.index_buffer = std::make_shared<Buffer>(context, sizeof(uint32_t) * mesh.indices.size(),
                                               VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                                               vk::BufferUsageFlagBits::eIndexBuffer);
  mesh.index_buffer->SetData(mesh.indices.data(), sizeof(uint32_t) * mesh.indices.size());

  return mesh;
}
