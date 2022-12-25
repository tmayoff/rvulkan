#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <vulkan/vulkan.hpp>

#include "VulkanContext.hpp"

class Buffer {
 public:
  Buffer() = default;
  Buffer(const VulkanContext& context, size_t byte_size, vk::MemoryPropertyFlags property_flags,
         vk::BufferUsageFlags buffer_usage);
  Buffer(const Buffer&) = delete;
  //   Buffer operator=(const Buffer&) = delete;

  ~Buffer();

  [[nodiscard]] const vk::Buffer& GetHandle() const { return buffer; }

  void SetData(void* data, uint32_t size);

 private:
  VulkanContext context;

  vk::Buffer buffer;
  vk::DeviceMemory device_memory;
  uint8_t* p_data;
};

#endif  // BUFFER_HPP_
