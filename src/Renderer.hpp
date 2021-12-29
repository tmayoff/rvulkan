#pragma once
#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <vulkan/vulkan.hpp>

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
  uint32_t currentFrameIndex;

  std::vector<VirtualFrame> virtualFrames;
};

#endif  // RENDERER_HPP_
