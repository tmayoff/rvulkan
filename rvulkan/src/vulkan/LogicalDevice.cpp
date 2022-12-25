#include "LogicalDevice.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <set>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

LogicalDevice::LogicalDevice(const PhysicalDevice& physical_device, const Surface& surface) {
  family_indices = FindQueueFamilies(physical_device, surface);

  std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {family_indices.graphics_family.value()};

  float queue_prio = 1.0F;
  for (const uint32_t& queue_family : unique_queue_families) {
    vk::DeviceQueueCreateInfo queue_info(vk::DeviceQueueCreateFlags(), queue_family, 1,
                                         &queue_prio);
    queue_create_infos.push_back(queue_info);
  }

  std::vector<const char*> device_extensions;
  device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  vk::DeviceCreateInfo create_info(vk::DeviceCreateFlagBits(), queue_create_infos, {},
                                   device_extensions, {});
  device = physical_device.GetHandle().createDevice(create_info);

  graphics_queue = device.getQueue(family_indices.graphics_family.value(), 0);
  present_queue = device.getQueue(family_indices.present_family.value(), 0);
}

QueueFamilyIndices LogicalDevice::FindQueueFamilies(const PhysicalDevice& physical_device,
                                                    const Surface& surface) {
  QueueFamilyIndices indices;

  auto queue_families = physical_device.GetHandle().getQueueFamilyProperties();
  int i = 0;
  for (const auto& q : queue_families) {
    if (q.queueFlags & vk::QueueFlagBits::eGraphics) indices.graphics_family = i;

    auto present_support = physical_device.GetHandle().getSurfaceSupportKHR(i, surface.GetHandle());
    if (present_support == VK_TRUE) indices.present_family = i;

    if (indices.IsComplete()) break;

    i++;
  }

  return indices;
}
