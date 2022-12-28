#ifndef RENDERPASS_HPP_
#define RENDERPASS_HPP_

#include <vulkan/vulkan.hpp>

#include "Core/types.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include "VulkanContext.hpp"

struct RenderPassState {
  vk::CommandBuffer Commands;
};

class RenderPass : public non_copyable, public non_movable {  // NOLINT
 public:
  RenderPass(const std::shared_ptr<VulkanContext> &context, const PipelineOptions &pipelineOptions);
  ~RenderPass();

  [[nodiscard]] const vk::RenderPass &GetHandle() const { return renderPass; }
  [[nodiscard]] const std::shared_ptr<Pipeline> &GetPipeline() const { return pipeline; }

 private:
  std::shared_ptr<VulkanContext> context;

  std::shared_ptr<Pipeline> pipeline;

  vk::RenderPass renderPass;
  vk::PipelineLayout layout;

  Shader shader;
};

#endif  // RENDERPASS_HPP_
