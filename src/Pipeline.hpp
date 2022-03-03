#pragma once
#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include <string>

#include "RenderPass.hpp"
#include "Shader.hpp"

struct PipelineInfo {
  Shader shader;
  RenderPass* renderPass;
  vk::Extent2D extent;
  vk::VertexInputBindingDescription vertexBindingDescription;
  std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions;
};

class Pipeline {
 public:
  Pipeline() = default;
  Pipeline(const PipelineInfo& info);

  auto GetHandle() const { return handle; }
  auto GetLayout() const { return pipelineLayout; }

 private:
  vk::Pipeline handle;
  vk::PipelineLayout pipelineLayout;
};

#endif  // PIPELINE_HPP_
