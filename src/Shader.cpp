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
  std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint8_t>> sources;
  for (auto shader : shaders) {
    sources[shader.stage] = ReadFile(shader.filepath);
    shaderFilepaths[shader.stage] = shader.filepath;
  }
  Compile(device, sources);
}

auto Shader::ReadFile(const std::string& path) -> std::vector<uint8_t> {
  std::fstream file;
  file.open(path, std::ios::in | std::ios::binary);

  if (file.is_open()) {
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(std::ios::beg);

    std::vector<char> source;
    source.resize(size);
    file.read(source.data(), size);
    return {source.begin(), source.end()};
  }

  return {};
}

void Shader::Compile(
    const vk::Device& device,
    const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint8_t>>& sources) {
  for (auto&& [stage, source] : sources) {
    vk::ShaderModuleCreateInfo createInfo;
    std::vector<uint32_t> code(source.cbegin(), source.cend());
    createInfo.setCode(code);
    shaderModules[stage] = device.createShaderModule(createInfo);
  }
}
