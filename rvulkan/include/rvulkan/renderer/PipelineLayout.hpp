#ifndef PIPELINELAYOUT_HPP
#define PIPELINELAYOUT_HPP

#include <string>
#include <vector>

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
    case ShaderDataType::Float: return 4;
    case ShaderDataType::Float2: return 4 * 2;
    case ShaderDataType::Float3: return 4 * 3;
    case ShaderDataType::Float4: return 4 * 4;
    case ShaderDataType::Mat3: return 4 * 3 * 3;
    case ShaderDataType::Mat4: return 4 * 3 * 4;
    case ShaderDataType::Int: return 4;
    case ShaderDataType::Int2: return 4 * 2;
    case ShaderDataType::Int3: return 4 * 3;
    case ShaderDataType::Int4: return 4 * 4;
    case ShaderDataType::Bool: return 1;
    default: return 0;
  }
}

class BufferElement {
 public:
  BufferElement(ShaderDataType type, std::string name)
      : name(std::move(name)), type(type), size(ShaderDataTypeSize(type)) {}

  [[nodiscard]] uint32_t GetComponentCount() const {
    switch (type) {
      case ShaderDataType::Float: return 1;
      case ShaderDataType::Float2: return 2;
      case ShaderDataType::Float3: return 3;
      case ShaderDataType::Float4: return 4;
      case ShaderDataType::Mat3: return 3;  // 3* float3
      case ShaderDataType::Mat4: return 4;  // 4* float4
      case ShaderDataType::Int: return 1;
      case ShaderDataType::Int2: return 2;
      case ShaderDataType::Int3: return 3;
      case ShaderDataType::Int4: return 4;
      case ShaderDataType::Bool: return 1;
      default: return 0;
    }
  }

  [[nodiscard]] const std::string& GetName() const { return name; }
  [[nodiscard]] const ShaderDataType& GetType() const { return type; }
  [[nodiscard]] uint32_t GetSize() const { return size; }

  void SetOffset(uint32_t offset) { this->offset = offset; }
  [[nodiscard]] size_t GetOffset() const { return offset; }

 private:
  std::string name;
  ShaderDataType type;
  uint32_t size;
  size_t offset{};
};

class BufferLayout {
 public:
  BufferLayout() = default;
  BufferLayout(std::initializer_list<BufferElement> elements) : elements(elements) {
    CalculateOffsetsAndStride();
  }

  [[nodiscard]] uint32_t GetStride() const { return stride; }
  [[nodiscard]] const std::vector<BufferElement>& GetElements() const { return elements; }

  std::vector<BufferElement>::iterator begin() { return elements.begin(); }
  std::vector<BufferElement>::iterator end() { return elements.end(); }

  [[nodiscard]] std::vector<BufferElement>::const_iterator begin() const {
    return elements.begin();
  }
  [[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return elements.end(); }

 private:
  void CalculateOffsetsAndStride() {
    size_t offset = 0;
    stride = 0;
    for (auto& element : elements) {
      element.SetOffset(offset);
      offset += element.GetSize();
      stride += element.GetSize();
    }
  }

  std::vector<BufferElement> elements;
  uint32_t stride = 0;
};

#endif  // PIPELINELAYOUT_HPP
