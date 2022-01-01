#pragma once
#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <vk_mem_alloc.h>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "RenderPass.hpp"

struct Vertex {
  glm::vec3 Position;
  glm::vec4 Color;
};

struct VirtualFrame {
  vk::CommandBuffer Commands;
  vk::Fence CommandQueueFence;
};

const std::vector<Vertex> vertices = {{{0.0f, -0.5f, 0.0f}, {0.5f, 0.0f, 0.5f, 1.0f}},
                                      {{0.5f, 0.5f, 0.0f}, {0.2f, 0.0f, 0.8f, 1.0f}},
                                      {{-0.5f, 0.5f, 0.0f}, {0.8f, 0.0f, 0.2f, 1.0f}}};

class Renderer {
 public:
  Renderer();

  void StartFrame();
  void EndFrame();

  const VirtualFrame& GetCurrentFrame() { return virtualFrames[currentFrameIndex]; }

 private:
  RenderPass renderPass;

  uint32_t currentFrameIndex = 0;
  uint32_t presentImageIndex = 0;

  vk::Buffer vertexBuffer;
  VmaAllocation alloc = {};

  std::vector<vk::Framebuffer> framebuffers;
  std::vector<VirtualFrame> virtualFrames;
};

#endif  // RENDERER_HPP_
