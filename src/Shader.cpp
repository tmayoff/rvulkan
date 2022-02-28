#include "Shader.hpp"

#include <fstream>
#include <shaderc/shaderc.hpp>

#include "Assert.hpp"

namespace Utils {

auto ShaderTypeFromString(const std::string& type) -> vk::ShaderStageFlagBits {
  if (type == "vertex") return vk::ShaderStageFlagBits::eVertex;
  if (type == "fragment" || type == "pixel") return vk::ShaderStageFlagBits::eFragment;
  assert(false);
}

auto VKShaderTypeToShaderC(vk::ShaderStageFlagBits type) -> shaderc_shader_kind {
  if (type == vk::ShaderStageFlagBits::eFragment)
    return shaderc_shader_kind::shaderc_fragment_shader;
  if (type == vk::ShaderStageFlagBits::eVertex) return shaderc_shader_kind::shaderc_vertex_shader;
  assert(false);
}

}  // namespace Utils

Shader::Shader(const vk::Device& device, const std::string& filepath) : filepath(filepath) {
  auto sources = PreProcess(ReadFile(filepath));
  Compile(device, sources);
}

Shader::Shader(const vk::Device& device, const std::string& vertexSource,
               const std::string& fragmentSource) {
  Compile(device, {{vk::ShaderStageFlagBits::eVertex, vertexSource},
                   {vk::ShaderStageFlagBits::eFragment, fragmentSource}});
}

auto Shader::ReadFile(const std::string& path) -> std::string {
  std::fstream file;
  file.open(path);

  if (file.is_open()) {
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(std::ios::beg);

    std::string source;
    source.resize(size);
    file.read(source.data(), size);
    return source;
  }

  return "";
}

auto Shader::PreProcess(const std::string& source)
    -> std::unordered_map<vk::ShaderStageFlagBits, std::string> {
  std::unordered_map<vk::ShaderStageFlagBits, std::string> shaderSources;

  const char* typeToken = "#type";
  size_t typeTokenLength = std::strlen(typeToken);
  auto pos = source.find(typeToken);  // Current pos initted to first shader type declaration
  while (pos != std::string::npos) {
    auto endOfLine = source.find_first_of("\n", pos);
    auto begShaderType = pos + typeTokenLength + 1;
    auto type = source.substr(begShaderType, endOfLine - begShaderType);

    auto nextLinePos = source.find_first_of("\n", endOfLine);
    pos = source.find(typeToken, nextLinePos);

    shaderSources[Utils::ShaderTypeFromString(type)] =
        (pos == std::string::npos) ? source.substr(nextLinePos)
                                   : source.substr(nextLinePos, pos - nextLinePos);
  }

  return shaderSources;
}

void Shader::Compile(const vk::Device& device,
                     const std::unordered_map<vk::ShaderStageFlagBits, std::string>& sources) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  for (auto&& [stage, source] : sources) {
    shaderc::SpvCompilationResult shaderModule = compiler.CompileGlslToSpv(
        source, Utils::VKShaderTypeToShaderC(stage), filepath.c_str(), options);
    if (shaderModule.GetCompilationStatus() != shaderc_compilation_status_success) assert(false);

    //
    vk::ShaderModuleCreateInfo createInfo;
    std::vector<uint32_t> code(shaderModule.cbegin(), shaderModule.cend());
    createInfo.setCode(code);
    shaderModules[stage] = device.createShaderModule(createInfo);
  }
}
