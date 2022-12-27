#ifndef MESH_HPP
#define MESH_HPP

#include <Buffer.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "Pipeline.hpp"

struct Vertex {
  glm::vec4 Position;
  glm::vec4 Color;
  glm::vec2 TexCoord;
  glm::vec3 Normal;

  static BufferLayout GetLayout() {
    return BufferLayout{
        {ShaderDataType::Float4, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float2, "a_TexCoord"},
        {ShaderDataType::Float3, "a_Normal"},
    };
  }

  static BufferLayout GetUniformLayout() {
    return BufferLayout{
        {ShaderDataType::Mat4, "u_ViewProjection"},
    };
  }
};

class Mesh {
 public:
  static Mesh CreateQuadMesh(const std::shared_ptr<VulkanContext>& context);

  [[nodiscard]] const std::vector<Vertex>& GetVertices() const { return vertices; }
  [[nodiscard]] const std::vector<uint32_t>& GetIndices() const { return indices; }

  [[nodiscard]] const std::shared_ptr<Buffer>& GetVertexBuffer() const { return vertex_buffer; }
  [[nodiscard]] const std::shared_ptr<Buffer>& GetIndexBuffer() const { return index_buffer; }

 private:
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  std::shared_ptr<Buffer> vertex_buffer;
  std::shared_ptr<Buffer> index_buffer;
};

#endif  // MESH_HPP
