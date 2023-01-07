#ifndef RENDERPASS_HPP_
#define RENDERPASS_HPP_

#include <rvulkan/core/types.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <vulkan/vulkan.hpp>

#include "Pipeline.hpp"
#include "Shader.hpp"

class RenderPass : public non_copyable, public non_movable {
 public:
  RenderPass(const std::shared_ptr<VulkanContext> &context, const PipelineOptions &pipelineOptions);
  ~RenderPass();

  [[nodiscard]] const vk::RenderPass &GetHandle() const { return renderPass; }
  [[nodiscard]] const std::shared_ptr<Pipeline> &GetPipeline() const { return pipeline; }

 private:
  vk::Device device;

  std::shared_ptr<Pipeline> pipeline;

  vk::RenderPass renderPass;
  vk::PipelineLayout layout;

  Shader shader;
};

#endif  // RENDERPASS_HPP_
