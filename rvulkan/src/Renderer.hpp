#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "Buffer.hpp"
#include "RenderPass.hpp"
#include "VulkanContext.hpp"
#include "scene/Components/MeshRenderer.hpp"

struct VirtualFrame {
  vk::CommandBuffer Commands;
  vk::Fence CommandQueueFence;
};

class Renderer {
 public:
  explicit Renderer(const VulkanContext& context);

  void StartFrame();
  void EndFrame();

  void DrawMesh(const Component::MeshRenderer& mesh_renderer);

  const VirtualFrame& GetCurrentFrame() { return virtualFrames[currentFrameIndex]; }

 private:
  VulkanContext context;

  RenderPass renderPass;

  uint32_t currentFrameIndex = 0;
  uint32_t presentImageIndex = 0;

  std::shared_ptr<Buffer> vertex_buffer;
  std::shared_ptr<Buffer> indexBuffer;

  std::vector<vk::Framebuffer> framebuffers;
  std::vector<VirtualFrame> virtualFrames;
};

#endif  // RENDERER_HPP_
