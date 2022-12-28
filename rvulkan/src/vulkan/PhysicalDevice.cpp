#include "PhysicalDevice.hpp"

PhysicalDevice::PhysicalDevice(const vk::Instance& instance) {
  auto devices = instance.enumeratePhysicalDevices();

  for (const auto& d : devices) {
    // TODO There should be more logic here
    physical_device = d;
    break;
  }
}
