#include "VulkanContext.hpp"

#include <vulkan/vulkan_core.h>

#include <Core/Log.hpp>
#include <Core/Memory.hpp>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan_handles.hpp>

VulkanContext::VulkanContext(const VulkanContextCreateOptions& options,
                             const std::shared_ptr<Window>& window) {
  vk::ApplicationInfo appInfo("rvulkan", VK_MAKE_VERSION(1, 0, 0), "rvulkan",  // NOLINT
                              VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_3);   // NOLINT

  vk::InstanceCreateInfo instance_info(vk::InstanceCreateFlags(), &appInfo, options.Layers,
                                       options.Extensions);
  instance = vk::createInstance(instance_info);

  // Init device
  surface = Surface(instance, window);
  physical_device = PhysicalDevice(instance, surface);
  surface.Init(physical_device);

  device = LogicalDevice(physical_device, surface);

  CreateAllocator();

  vk::CommandPoolCreateInfo command_pool_info;
  command_pool_info
      .setQueueFamilyIndex(device.GetIndices().graphics_family.value())  // NOLINT
      .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
                vk::CommandPoolCreateFlagBits::eTransient);
  commandPool = device.GetHandle().createCommandPool(command_pool_info);
}

void VulkanContext::CreateAllocator() {
  VmaAllocatorCreateInfo allocator_info{};
  allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;  // NOLINT
  allocator_info.instance = instance;
  allocator_info.physicalDevice = physical_device.GetHandle();
  allocator_info.device = device.GetHandle();
  vmaCreateAllocator(&allocator_info, &allocator);
}
