#pragma once
#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <filesystem>
#include <string>
#include <vulkan/vulkan.hpp>

class Shader {
 public:
  Shader() = default;
  Shader(const vk::Device& device, const std::string& filepath);
  Shader(const vk::Device& device, const std::string& vertexSource,
         const std::string& fragmentSource);

  vk::ShaderModule GetShaderModule(vk::ShaderStageFlagBits shaderStage) {
    return shaderModules[shaderStage];
  }

 private:
  static auto ReadFile(const std::string& path) -> std::string;
  static auto PreProcess(const std::string& source)
      -> std::unordered_map<vk::ShaderStageFlagBits, std::string>;

  void Compile(const vk::Device& device,
               const std::unordered_map<vk::ShaderStageFlagBits, std::string>& sources);

  std::filesystem::path filepath;

  std::unordered_map<vk::ShaderStageFlagBits, vk::ShaderModule> shaderModules;
};

#endif  // SHADER_HPP_
