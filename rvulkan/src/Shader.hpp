#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <vulkan/vulkan.hpp>

class VulkanContext;

class Shader {
 public:
  static std::vector<uint32_t> ReadFile(const std::string& filepath);

  Shader() = default;
  Shader(const std::shared_ptr<VulkanContext>& context, const std::vector<uint32_t>& vertCode,
         const std::vector<uint32_t>& fragCode);

  [[nodiscard]] const vk::ShaderModule& GetVertexModule() const { return vertexShader; }
  [[nodiscard]] const vk::ShaderModule& GetFragmentModule() const { return fragmentShader; }

 private:
  vk::ShaderModule vertexShader;
  vk::ShaderModule fragmentShader;
};

#endif  // SHADER_HPP_
