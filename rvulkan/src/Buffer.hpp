#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <vulkan/vulkan.hpp>

#include "VulkanContext.hpp"

class Buffer {
 public:
  Buffer() = default;
  Buffer(const VulkanContext& context, size_t byte_size, VmaMemoryUsage memory_usage,
         vk::BufferUsageFlags buffer_usage);

  // Create copy constructors
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  ~Buffer();

  [[nodiscard]] const vk::Buffer& GetHandle() const { return buffer; }

  void SetData(void* data, uint32_t size);

 private:
  VmaAllocator allocator{};

  vk::Buffer buffer;
  VmaAllocation allocation = {};
  uint8_t* memory = nullptr;
};

#endif  // BUFFER_HPP_
