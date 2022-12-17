#include "Buffer.hpp"

#include "VulkanContext.hpp"

Buffer::Buffer(size_t byteSize, VmaMemoryUsage memoryUsage, vk::BufferUsageFlags bufferUsage) {
  auto& vulkanContext = GetCurrentVulkanContext();

  vk::BufferCreateInfo bufferInfo{};
  bufferInfo.setSize(vk::DeviceSize(byteSize))
      .setUsage(bufferUsage)
      .setSharingMode(vk::SharingMode::eExclusive);

  buffer = vulkanContext.GetDevice().createBuffer(bufferInfo);

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = memoryUsage;
  vmaCreateBuffer(vulkanContext.GetAllocator(), (VkBufferCreateInfo*)&bufferInfo, &allocInfo,
                  (VkBuffer*)&buffer, &allocation, nullptr);
}

void Buffer::SetData(void* data, uint32_t size) {
  void* mem = nullptr;
  vmaMapMemory(GetCurrentVulkanContext().GetAllocator(), allocation, &mem);
  std::memcpy(mem, data, size);
  vmaUnmapMemory(GetCurrentVulkanContext().GetAllocator(), allocation);
  memory = (uint8_t*)mem;
}
