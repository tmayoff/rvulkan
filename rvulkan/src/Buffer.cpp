#include "Buffer.hpp"

#include <rvulkan/core/memory.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/vulkan_structs.hpp>

Buffer::Buffer(const std::shared_ptr<VulkanContext>& context, size_t byte_size,
               VmaMemoryUsage memory_usage, vk::BufferUsageFlags buffer_usage)
    : allocator(context->GetAllocator()) {
  vk::BufferCreateInfo buffer_info{};
  buffer_info.setSize(static_cast<vk::DeviceSize>(byte_size))
      .setUsage(buffer_usage)
      .setSharingMode(vk::SharingMode::eExclusive);

  buffer = context->GetLogicalDevice()->GetHandle().createBuffer(buffer_info);

  VmaAllocationCreateInfo alloc_info = {};
  alloc_info.usage = memory_usage;
  alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
  vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&buffer_info), &alloc_info,
                  reinterpret_cast<VkBuffer*>(&buffer), &allocation, nullptr);
}

Buffer::~Buffer() { vmaDestroyBuffer(allocator, buffer, allocation); }

void Buffer::SetData(void* data, uint32_t size) {
  void* mem = nullptr;
  vmaMapMemory(allocator, allocation, &mem);
  std::memcpy(mem, data, size);
  vmaUnmapMemory(allocator, allocation);
  memory = static_cast<uint8_t*>(mem);
}
