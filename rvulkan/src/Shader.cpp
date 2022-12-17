#include "Shader.hpp"

#include <fstream>
#include <string>

#include "VulkanContext.hpp"

std::vector<uint32_t> Shader::ReadFile(const std::string& filepath) {
  std::vector<uint32_t> byteCode;
  std::ifstream file(filepath, std::ios_base::binary);
  auto binaryData =
      std::vector<char>(std::istreambuf_iterator(file), std::istreambuf_iterator<char>());
  byteCode.resize(binaryData.size() / sizeof(uint32_t));
  std::copy((uint32_t*)binaryData.data(), (uint32_t*)(binaryData.data() + binaryData.size()),
            byteCode.begin());
  return byteCode;
}

Shader::Shader(const std::vector<uint32_t>& vertCode, const std::vector<uint32_t>& fragCode) {
  auto& vulkan = GetCurrentVulkanContext();

  vk::ShaderModuleCreateInfo vertShaderInfo;
  vertShaderInfo.setCode(vertCode);
  vertexShader = vulkan.GetDevice().createShaderModule(vertShaderInfo);

  vk::ShaderModuleCreateInfo fragShaderInfo;
  fragShaderInfo.setCode(fragCode);
  fragmentShader = vulkan.GetDevice().createShaderModule(fragShaderInfo);
}
