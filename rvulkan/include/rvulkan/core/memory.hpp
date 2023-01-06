#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <vk_mem_alloc.h>

#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

vk::DeviceMemory AllocateDeviceMemory(const vk::Device& device,
                                      const vk::PhysicalDeviceMemoryProperties& mem_properties,
                                      const vk::MemoryRequirements& mem_reqs,
                                      const vk::MemoryPropertyFlags& mem_flags);

uint32_t FindMemoryType(const vk::PhysicalDeviceMemoryProperties& mem_props, uint32_t type_bits,
                        const vk::MemoryPropertyFlags& requirement_mask);

#endif  // MEMORY_HPP
