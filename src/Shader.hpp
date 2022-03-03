#pragma once
#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <filesystem>
#include <string>
#include <vulkan/vulkan.hpp>

struct ShaderInfo {
  vk::ShaderStageFlagBits stage;
  std::filesystem::path filepath;
};

class Shader {
 public:
  Shader() = default;
  Shader(const vk::Device& device, std::vector<ShaderInfo> shaders);
  Shader(const vk::Device& device, const std::string& vertexSource,
         const std::string& fragmentSource);

  auto GetShaderModules() const { return shaderModules; }
  auto GetShaderModule(vk::ShaderStageFlagBits shaderStage) { return shaderModules[shaderStage]; }

 private:
  static auto ReadFile(const std::string& path) -> std::string;

  void Compile(const vk::Device& device,
               const std::unordered_map<vk::ShaderStageFlagBits, std::string>& sources);

  std::unordered_map<vk::ShaderStageFlagBits, vk::ShaderModule> shaderModules;
  std::unordered_map<vk::ShaderStageFlagBits, std::filesystem::path> shaderFilepaths;
};

#endif  // SHADER_HPP_
