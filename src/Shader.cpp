#include "Shader.hpp"

#include <fstream>
#include <shaderc/shaderc.hpp>

#include "Assert.hpp"

namespace Utils {

auto VKShaderTypeToShaderC(vk::ShaderStageFlagBits type) -> shaderc_shader_kind {
  if (type == vk::ShaderStageFlagBits::eFragment)
    return shaderc_shader_kind::shaderc_fragment_shader;
  if (type == vk::ShaderStageFlagBits::eVertex) return shaderc_shader_kind::shaderc_vertex_shader;
  assert(false);
}

}  // namespace Utils

Shader::Shader(const vk::Device& device, std::vector<ShaderInfo> shaders) {
  std::unordered_map<vk::ShaderStageFlagBits, std::vector<char>> sources;
  for (auto shader : shaders) {
    sources[shader.stage] = ReadFile(shader.filepath);
    shaderFilepaths[shader.stage] = shader.filepath;
  }
  Compile(device, sources);
}

auto Shader::ReadFile(const std::string& path) -> std::vector<char> {
  std::ifstream file;

  file.open(path, std::ios::binary | std::ios::ate);
  assert(file.is_open());

  auto size = file.tellg();
  assert(size > 0);
  file.seekg(0, std::ios::beg);

  std::vector<char> source;
  source.resize(size);
  file.read(source.data(), size);
  return source;
}

void Shader::Compile(
    const vk::Device& device,
    const std::unordered_map<vk::ShaderStageFlagBits, std::vector<char>>& sources) {
  for (auto&& [stage, source] : sources) {
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setCodeSize(source.size());
    createInfo.setPCode(reinterpret_cast<const uint32_t*>(source.data()));
    shaderModules[stage] = device.createShaderModule(createInfo);
  }
}
