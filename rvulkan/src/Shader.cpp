#include "Shader.hpp"

#include <fstream>
#include <rvulkan/vulkan_context.hpp>
#include <string>
#include <vulkan/LogicalDevice.hpp>

std::vector<uint32_t> Shader::ReadFile(const std::string& filepath) {
  std::vector<uint32_t> byte_code;
  std::ifstream file(filepath, std::ios_base::binary);
  auto binary_data =
      std::vector<char>(std::istreambuf_iterator(file), std::istreambuf_iterator<char>());
  byte_code.resize(binary_data.size() / sizeof(uint32_t));
  std::copy(reinterpret_cast<uint32_t*>(binary_data.data()),
            reinterpret_cast<uint32_t*>(binary_data.data() + binary_data.size()),
            byte_code.begin());
  return byte_code;
}

Shader::Shader(const vk::Device& device, const ShaderCode& code) : device_context(device) {
  vk::ShaderModuleCreateInfo vert_shader_info;
  vert_shader_info.setCode(code.vertex_code);
  vertex_module = device.createShaderModule(vert_shader_info);

  vk::ShaderModuleCreateInfo frag_shader_info;
  frag_shader_info.setCode(code.fragment_code);
  fragment_module = device.createShaderModule(frag_shader_info);
}

Shader::~Shader() {
  device_context.destroyShaderModule(vertex_module);
  device_context.destroyShaderModule(fragment_module);
}
