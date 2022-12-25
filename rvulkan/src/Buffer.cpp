#include "Buffer.hpp"

#include <Core/Memory.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "VulkanContext.hpp"

Buffer::Buffer(const VulkanContext& context, size_t byte_size, VmaMemoryUsage memory_usage,
               vk::BufferUsageFlags buffer_usage)
    : allocator(context.GetAllocator()) {
  vk::BufferCreateInfo bufferInfo{};
  bufferInfo.setSize(static_cast<vk::DeviceSize>(byte_size))
      .setUsage(buffer_usage)
      .setSharingMode(vk::SharingMode::eExclusive);

  buffer = context.GetLogicalDevice().GetHandle().createBuffer(bufferInfo);

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = memory_usage;
  allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
  vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&bufferInfo), &allocInfo,
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
