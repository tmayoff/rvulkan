#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include <rvulkan/core/types.hpp>
#include <rvulkan/renderer/PipelineLayout.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "Shader.hpp"

struct PipelineOptions {
  vk::Extent2D surface_extent;
  Shader shader;
  BufferLayout bufferLayout;
  std::vector<BufferLayout> uniform_buffer_layouts;
};

class Pipeline : public non_copyable, non_movable {
 public:
  Pipeline(const std::shared_ptr<VulkanContext>& context, const PipelineOptions& options,
           const vk::RenderPass& renderPass);

  ~Pipeline();

  [[nodiscard]] const vk::Pipeline& GetHandle() const { return pipeline; }
  [[nodiscard]] const vk::PipelineLayout& GetLayout() const { return layout; }

 private:
  std::shared_ptr<VulkanContext> context;

  vk::Pipeline pipeline;
  vk::PipelineLayout layout;
};

#endif  // PIPELINE_HPP_
