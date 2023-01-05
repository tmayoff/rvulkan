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

Shader::Shader(const std::shared_ptr<VulkanContext>& context, const std::vector<uint32_t>& vertCode,
               const std::vector<uint32_t>& fragCode) {
  vk::ShaderModuleCreateInfo vertShaderInfo;
  vertShaderInfo.setCode(vertCode);
  vertexShader = context->GetLogicalDevice()->GetHandle().createShaderModule(vertShaderInfo);

  vk::ShaderModuleCreateInfo fragShaderInfo;
  fragShaderInfo.setCode(fragCode);
  fragmentShader = context->GetLogicalDevice()->GetHandle().createShaderModule(fragShaderInfo);
}
