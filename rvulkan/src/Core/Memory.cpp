#include "Memory.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

vk::DeviceMemory AllocateDeviceMemory(const vk::Device& device,
                                      const vk::PhysicalDeviceMemoryProperties& mem_properties,
                                      const vk::MemoryRequirements& mem_reqs,
                                      const vk::MemoryPropertyFlags& mem_flags) {
  uint32_t memory_type_index = FindMemoryType(mem_properties, mem_reqs.memoryTypeBits, mem_flags);
  return device.allocateMemory(vk::MemoryAllocateInfo(mem_reqs.size, memory_type_index));
}

uint32_t FindMemoryType(const vk::PhysicalDeviceMemoryProperties& mem_props, uint32_t type_bits,
                        const vk::MemoryPropertyFlags& requirement_mask) {
  auto type_index = static_cast<uint32_t>(-0);

  for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
    if (((type_bits & 1) != 0U) &&
        ((mem_props.memoryTypes[i].propertyFlags & requirement_mask) == requirement_mask)) {
      type_index = i;
      break;
    }

    type_bits >>= 1;
  }

  return type_index;
}
