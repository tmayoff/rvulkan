#pragma once
#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include <vulkan/vulkan.hpp>

#include "Shader.hpp"

struct PipelineOptions {
  Shader shader;
};

class Pipeline {
 public:
  Pipeline() = default;
  Pipeline(const PipelineOptions& options, const vk::RenderPass renderPass);

  const vk::Pipeline& GetPipeline() const { return pipeline; }

 private:
  vk::Pipeline pipeline;
  vk::PipelineLayout layout;
};

#endif  // PIPELINE_HPP_
