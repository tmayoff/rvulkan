#ifndef SHADER_HPP
#define SHADER_HPP

#include <rvulkan/core/types.hpp>
#include <vulkan/vulkan.hpp>

class VulkanContext;

struct ShaderCode {
  std::vector<uint32_t> vertex_code;
  std::vector<uint32_t> fragment_code;
};

class Shader : public non_copyable, public non_movable {
 public:
  static std::vector<uint32_t> ReadFile(const std::string& filepath);

  Shader() = default;
  Shader(const vk::Device& device, const ShaderCode& code);
  ~Shader();

  [[nodiscard]] const vk::ShaderModule& GetVertexModule() const { return vertex_module; }
  [[nodiscard]] const vk::ShaderModule& GetFragmentModule() const { return fragment_module; }

 private:
  vk::Device device_context;
  vk::ShaderModule vertex_module;
  vk::ShaderModule fragment_module;
};

#endif  // SHADER_HPP
