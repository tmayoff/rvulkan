#include "Buffer.hpp"

#include <Core/Memory.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "VulkanContext.hpp"

Buffer::Buffer(const VulkanContext& context, size_t byte_size,
               vk::MemoryPropertyFlags property_flags, vk::BufferUsageFlags buffer_usage)
    : context(context) {
  auto device = context.GetLogicalDevice();
  auto phys_device = context.GetPhysicalDevice();

  buffer = device.GetHandle().createBuffer(
      vk::BufferCreateInfo(vk::BufferCreateFlags(), byte_size, buffer_usage));

  // Allocate Device memory
  device_memory =
      AllocateDeviceMemory(device.GetHandle(), phys_device.GetHandle().getMemoryProperties(),
                           device.GetHandle().getBufferMemoryRequirements(buffer), property_flags);
}

Buffer::~Buffer() {}

void Buffer::SetData(void* data, uint32_t size) {
  auto device = context.GetLogicalDevice().GetHandle();
  auto* p = device.mapMemory(device_memory, 0, size);
  std::memcpy(p, data, size);
  device.unmapMemory(device_memory);
  device.bindBufferMemory(buffer, device_memory, 0);
}
