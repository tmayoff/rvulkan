#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <vulkan/vulkan.hpp>

class VulkanContext;

struct ShaderCode {
  std::vector<uint32_t> vertex_code;
  std::vector<uint32_t> fragment_code;
};

class Shader {
 public:
  static std::vector<uint32_t> ReadFile(const std::string& filepath);

  Shader() = default;
  Shader(const vk::Device& device, const ShaderCode& code);

  [[nodiscard]] const vk::ShaderModule& GetVertexModule() const { return vertexShader; }
  [[nodiscard]] const vk::ShaderModule& GetFragmentModule() const { return fragmentShader; }

 private:
  vk::ShaderModule vertexShader;
  vk::ShaderModule fragmentShader;
};

#endif  // SHADER_HPP_
