#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "Buffer.hpp"
#include "RenderPass.hpp"
#include "VulkanContext.hpp"

struct Vertex {
  glm::vec3 Position;
  glm::vec4 Color;
};

struct VirtualFrame {
  vk::CommandBuffer Commands;
  vk::Fence CommandQueueFence;
};

const std::array<glm::vec3, 4> QuadVertexPositions = {
    glm::vec3{-0.5F, -0.5F, 0.0F}, glm::vec3{0.5F, -0.5F, 0.0F}, glm::vec3{0.5F, 0.5F, 0.0F},
    glm::vec3{-0.5F, 0.5F, 0.0F}};

constexpr size_t QuadVertexCount = 4;
constexpr std::array<uint32_t, 6> QuadIndices = {0, 1, 2, 2, 3, 0};

class Renderer {
 public:
  explicit Renderer(const VulkanContext& context);

  void StartFrame();
  void EndFrame();

  void DrawQuad();

  const VirtualFrame& GetCurrentFrame() { return virtualFrames[currentFrameIndex]; }

 private:
  VulkanContext context;

  RenderPass renderPass;

  uint32_t currentFrameIndex = 0;
  uint32_t presentImageIndex = 0;

  Buffer vertex_buffer;
  Buffer indexBuffer;

  std::vector<vk::Framebuffer> framebuffers;
  std::vector<VirtualFrame> virtualFrames;
};

#endif  // RENDERER_HPP_
