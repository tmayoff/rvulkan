#pragma once
#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include <vulkan/vulkan.hpp>

#include "Shader.hpp"

enum class ShaderDataType {
  None = 0,
  Float,
  Float2,
  Float3,
  Float4,
  Mat3,
  Mat4,
  Int,
  Int2,
  Int3,
  Int4,
  Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type) {
  switch (type) {
    case ShaderDataType::Float:
      return 4;
    case ShaderDataType::Float2:
      return 4 * 2;
    case ShaderDataType::Float3:
      return 4 * 3;
    case ShaderDataType::Float4:
      return 4 * 4;
    case ShaderDataType::Mat3:
      return 4 * 3 * 3;
    case ShaderDataType::Mat4:
      return 4 * 3 * 4;
    case ShaderDataType::Int:
      return 4;
    case ShaderDataType::Int2:
      return 4 * 2;
    case ShaderDataType::Int3:
      return 4 * 3;
    case ShaderDataType::Int4:
      return 4 * 4;
    case ShaderDataType::Bool:
      return 1;
    default:
      return 0;
  }
}

static vk::Format ShaderDataTypeToVkForamt(ShaderDataType type) {
  switch (type) {
    case ShaderDataType::Float:
      return vk::Format::eR32Sfloat;
    case ShaderDataType::Float2:
      return vk::Format::eR32G32Sfloat;
    case ShaderDataType::Float3:
      return vk::Format::eR32G32B32Sfloat;
    case ShaderDataType::Float4:
      return vk::Format::eR32G32B32A32Sfloat;
    case ShaderDataType::Int:
      return vk::Format::eR32Sint;
    case ShaderDataType::Int2:
      return vk::Format::eR32G32Sint;
    case ShaderDataType::Int3:
      return vk::Format::eR32G32B32Sint;
    case ShaderDataType::Int4:
      return vk::Format::eR32G32B32A32Sint;
    case ShaderDataType::Bool:
      return vk::Format::eR32Sint;
    default:
      return vk::Format::eUndefined;
  }
}

struct BufferElement {
  std::string Name;
  ShaderDataType Type;
  uint32_t Size;
  size_t Offset;
  bool Normalized;

  BufferElement() = default;
  BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
      : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

  uint32_t GetComponentCount() const {
    switch (Type) {
      case ShaderDataType::Float:
        return 1;
      case ShaderDataType::Float2:
        return 2;
      case ShaderDataType::Float3:
        return 3;
      case ShaderDataType::Float4:
        return 4;
      case ShaderDataType::Mat3:
        return 3;  // 3* float3
      case ShaderDataType::Mat4:
        return 4;  // 4* float4
      case ShaderDataType::Int:
        return 1;
      case ShaderDataType::Int2:
        return 2;
      case ShaderDataType::Int3:
        return 3;
      case ShaderDataType::Int4:
        return 4;
      case ShaderDataType::Bool:
        return 1;
      default:
        return 0;
    }
  }
};

class BufferLayout {
 public:
  BufferLayout() {}
  BufferLayout(std::initializer_list<BufferElement> elements) : elements(elements) {
    CalculateOffsetsAndStride();
  }

  uint32_t GetStride() const { return stride; }
  const std::vector<BufferElement>& GetElements() const { return elements; }

  std::vector<BufferElement>::iterator begin() { return elements.begin(); }
  std::vector<BufferElement>::iterator end() { return elements.end(); }
  std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
  std::vector<BufferElement>::const_iterator end() const { return elements.end(); }

 private:
  void CalculateOffsetsAndStride() {
    size_t offset = 0;
    stride = 0;
    for (auto& element : elements) {
      element.Offset = offset;
      offset += element.Size;
      stride += element.Size;
    }
  }

  std::vector<BufferElement> elements;
  uint32_t stride = 0;
};

struct PipelineOptions {
  Shader shader;
  BufferLayout bufferLayout;
};

class Pipeline {
 public:
  Pipeline() = default;
  Pipeline(const PipelineOptions& options, const vk::RenderPass renderPass);

  const vk::Pipeline& GetPipeline() const { return pipeline; }

 private:
  vk::Pipeline pipeline;
  vk::PipelineLayout layout;
};

#endif  // PIPELINE_HPP_
