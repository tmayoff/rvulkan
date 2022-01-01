#pragma once
#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <vulkan/vulkan.hpp>

#include "RenderPass.hpp"

struct VirtualFrame {
  vk::CommandBuffer Commands;
  vk::Fence CommandQueueFence;
};

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

  std::vector<vk::Framebuffer> framebuffers;

  std::vector<VirtualFrame> virtualFrames;
};

#endif  // RENDERER_HPP_
