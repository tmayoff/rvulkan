#pragma once
#ifndef RENDERPASS_HPP_
#define RENDERPASS_HPP_

#include <vulkan/vulkan.hpp>

struct RenderPassInfo {
  vk::Format colorAttachmentFormat;
};

class RenderPass {
 public:
  RenderPass() = default;
  RenderPass(const RenderPassInfo& info);
  ~RenderPass();

  auto GetHandle() const -> vk::RenderPass { return handle; }

 private:
  vk::RenderPass handle;
};

#endif  // RENDERPASS_HPP_
