#pragma once
#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <fstream>
#include <vector>
#include <vulkan/vulkan.hpp>

class Renderer {
  static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to open shader file");

    int64_t fileSize = (int64_t)file.tellg();
    std::vector<char> buf(fileSize);
    file.seekg(0);
    file.read(buf.data(), fileSize);
    file.close();
    return buf;
  }

 public:
  Renderer(const vk::Device &device);

 private:
  void createGraphicsPipeline();
  void createRenderPass();
  vk::ShaderModule createShaderModule(const std::vector<char> &code);

  vk::Device device;
  vk::RenderPass renderPass;
  vk::PipelineLayout pipelineLayout;
  vk::Pipeline graphicsPipeline;
};

#endif  // RENDERER_HPP_
