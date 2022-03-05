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

  auto GetShaderModules() const { return shaderModules; }
  auto GetShaderModule(vk::ShaderStageFlagBits shaderStage) { return shaderModules[shaderStage]; }

 private:
  static auto ReadFile(const std::string& path) -> std::vector<uint8_t>;

  void Compile(const vk::Device& device,
               const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint8_t>>& sources);

  std::unordered_map<vk::ShaderStageFlagBits, vk::ShaderModule> shaderModules;
  std::unordered_map<vk::ShaderStageFlagBits, std::filesystem::path> shaderFilepaths;
};

#endif  // SHADER_HPP_
