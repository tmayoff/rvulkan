#include "LogicalDevice.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <set>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "vulkan/PhysicalDevice.hpp"

LogicalDevice::LogicalDevice(const PhysicalDevice& physical_device, const Surface& surface)
    : family_indices(PhysicalDevice::FindQueueFamilies(physical_device.GetHandle(), surface)) {
  std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {family_indices.graphics_family.value(),
                                              family_indices.present_family.value()};

  float queue_prio = 1.0F;
  for (const uint32_t& queue_family : unique_queue_families) {
    vk::DeviceQueueCreateInfo queue_info({}, queue_family, 1, &queue_prio);
    queue_create_infos.push_back(queue_info);
  }

  std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  std::vector<const char*> device_layers = {"VK_LAYER_KHRONOS_validation"};
  vk::DeviceCreateInfo create_info({}, queue_create_infos, device_layers, device_extensions, {});
  device = physical_device.GetHandle().createDevice(create_info);

  graphics_queue = device.getQueue(family_indices.graphics_family.value(), 0);
  present_queue = device.getQueue(family_indices.present_family.value(), 0);
}
