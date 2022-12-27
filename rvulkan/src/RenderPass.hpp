#pragma once
#include "VulkanContext.hpp"
#ifndef RENDERPASS_HPP_
#define RENDERPASS_HPP_

#include <vulkan/vulkan.hpp>

#include "Pipeline.hpp"
#include "Shader.hpp"

struct RenderPassState {
  vk::CommandBuffer Commands;
};

class RenderPass {
 public:
  RenderPass() = default;
  RenderPass(const VulkanContext &context, const PipelineOptions &pipelineOptions);

  void Render(const RenderPassState &state);

  [[nodiscard]] const vk::RenderPass &GetRenderPass() const { return renderPass; }
  [[nodiscard]] const Pipeline &GetPipeline() const { return pipeline; }

 private:
  Pipeline pipeline;

  vk::RenderPass renderPass;
  vk::PipelineLayout layout;

  Shader shader;
};

#endif  // RENDERPASS_HPP_
