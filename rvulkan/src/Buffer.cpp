#include "Buffer.hpp"

#include "VulkanContext.hpp"

Buffer::Buffer(size_t byte_size, VmaMemoryUsage memory_usage, vk::BufferUsageFlags buffer_usage) {
  auto& vulkanContext = GetCurrentVulkanContext();

  vk::BufferCreateInfo bufferInfo{};
  bufferInfo.setSize(static_cast<vk::DeviceSize>(byte_size))
      .setUsage(buffer_usage)
      .setSharingMode(vk::SharingMode::eExclusive);

  buffer = vulkanContext.GetDevice().createBuffer(bufferInfo);

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = memory_usage;
  vmaCreateBuffer(vulkanContext.GetAllocator(), reinterpret_cast<VkBufferCreateInfo*>(&bufferInfo),
                  &allocInfo, reinterpret_cast<VkBuffer*>(&buffer), &allocation, nullptr);
}

Buffer::~Buffer() {
  vmaDestroyBuffer(GetCurrentVulkanContext().GetAllocator(), buffer, allocation);
}

void Buffer::SetData(void* data, uint32_t size) {
  void* mem = nullptr;
  vmaMapMemory(GetCurrentVulkanContext().GetAllocator(), allocation, &mem);
  std::memcpy(mem, data, size);
  vmaUnmapMemory(GetCurrentVulkanContext().GetAllocator(), allocation);
  memory = static_cast<uint8_t*>(mem);
}
