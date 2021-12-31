#pragma once
#ifndef RENDERPASS_HPP_
#define RENDERPASS_HPP_

#include <vulkan/vulkan.hpp>

#include "Shader.hpp"

class RenderPass {
 public:
  RenderPass();

  const vk::RenderPass &GetRenderPass() const { return renderPass; }
  const vk::Pipeline &GetPipeline() const { return pipeline; }
  const vk::Framebuffer &GetFramebuffer() const { return framebuffer; }

  void createPipeline();

 private:
  vk::RenderPass renderPass;
  vk::Pipeline pipeline;
  vk::PipelineLayout layout;
  vk::Framebuffer framebuffer;

  Shader shader;
};

#endif  // RENDERPASS_HPP_
