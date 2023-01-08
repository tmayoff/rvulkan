#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <rvulkan/core/types.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <vulkan/vulkan.hpp>

class Buffer : public non_copyable, public non_movable {
 public:
  Buffer() = default;
  Buffer(const std::shared_ptr<VulkanContext>& context, size_t byte_size,
         VmaMemoryUsage memory_usage, vk::BufferUsageFlags buffer_usage);

  ~Buffer();

  [[nodiscard]] const vk::Buffer& GetHandle() const { return buffer; }

  void SetData(void* data, uint32_t size);

 private:
  int buffer_object = 0;
  VmaAllocator allocator{};

  vk::Buffer buffer;
  VmaAllocation allocation = {};
  uint8_t* memory = nullptr;
};

#endif  // BUFFER_HPP_
