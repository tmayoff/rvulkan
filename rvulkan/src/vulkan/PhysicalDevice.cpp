#include "PhysicalDevice.hpp"

#include <core/log.hpp>
#include <set>
#include <string>
#include <vulkan/vulkan_enums.hpp>

#include "Surface.hpp"

PhysicalDevice::PhysicalDevice(const vk::Instance& instance, const Surface& surface) {
  auto devices = instance.enumeratePhysicalDevices();

  for (const auto& device : devices) {
    if (IsDeviceSuitable(device, surface)) {
      physical_device = device;
      break;
    }
  }
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilies(const vk::PhysicalDevice& physical_device,
                                                     const Surface& surface) {
  QueueFamilyIndices indices;

  auto queue_families = physical_device.getQueueFamilyProperties();

  int i = 0;
  for (const auto& q : queue_families) {
    if (q.queueFlags & vk::QueueFlagBits::eGraphics) indices.graphics_family = i;

    auto present_support = physical_device.getSurfaceSupportKHR(i, surface.GetHandle());
    if (present_support == VK_TRUE) indices.present_family = i;

    if (indices.IsComplete()) break;

    i++;
  }

  return indices;
}

SwapchainSupportDetails PhysicalDevice::QuerySwapchainSupportDetails(
    const vk::PhysicalDevice& device, const Surface& surface) {
  SwapchainSupportDetails details;

  details.capabilities = device.getSurfaceCapabilitiesKHR(surface.GetHandle());
  details.formats = device.getSurfaceFormatsKHR(surface.GetHandle());
  details.present_modes = device.getSurfacePresentModesKHR(surface.GetHandle());

  return details;
}

bool PhysicalDevice::CheckExtensionSupport(const vk::PhysicalDevice& device) {
  auto available_extensions = device.enumerateDeviceExtensionProperties();

  std::set<std::string_view> required_extension = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  for (const auto& e : available_extensions) required_extension.erase(e.extensionName);

  return required_extension.empty();
}

bool PhysicalDevice::IsDeviceSuitable(const vk::PhysicalDevice& device, const Surface& surface) {
  auto indices = FindQueueFamilies(device, surface);

  bool extensions_supported = CheckExtensionSupport(device);

  bool swapchain_adequate = false;
  if (extensions_supported) {
    auto details = QuerySwapchainSupportDetails(device, surface);
    swapchain_adequate = !details.formats.empty() && !details.present_modes.empty();
  }

  return indices.IsComplete() && extensions_supported && swapchain_adequate;
}
