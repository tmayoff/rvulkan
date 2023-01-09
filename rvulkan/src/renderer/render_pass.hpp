#ifndef RENDER_PASS_HPP
#define RENDER_PASS_HPP

#include <rvulkan/core/types.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <vulkan/vulkan.hpp>

#include "Pipeline.hpp"
#include "Shader.hpp"

class RenderPass : public non_copyable, public non_movable {
 public:
  RenderPass(const std::shared_ptr<VulkanContext> &context, std::unique_ptr<Shader> &&,
             const PipelineOptions &pipelineOptions);
  ~RenderPass();

  [[nodiscard]] const vk::RenderPass &GetHandle() const { return renderPass; }
  [[nodiscard]] const std::shared_ptr<Pipeline> &GetPipeline() const { return pipeline; }

 private:
  vk::Device device;

  std::shared_ptr<Pipeline> pipeline;

  vk::RenderPass renderPass;
  vk::PipelineLayout layout;

  std::unique_ptr<Shader> shader;
};

#endif  // RENDER_PASS_HPP
