#ifndef PHYSICALDEVICE_HPP
#define PHYSICALDEVICE_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

struct DeviceSelectionOptions {};

class PhysicalDevice {
 public:
  PhysicalDevice() = default;
  explicit PhysicalDevice(const vk::Instance& instance);

  [[nodiscard]] const vk::PhysicalDevice& GetHandle() const { return physical_device; }

 private:
  vk::PhysicalDevice physical_device;
};

#endif  // PHYSICALDEVICE_HPP
