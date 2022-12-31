#ifndef LOGICALDEVICE_HPP
#define LOGICALDEVICE_HPP

#include <optional>
#include <vulkan/vulkan.hpp>

#include "PhysicalDevice.hpp"
#include "Surface.hpp"

class LogicalDevice {
 public:
  LogicalDevice() = default;
  explicit LogicalDevice(const PhysicalDevice& physical_device, const Surface& surface);

  [[nodiscard]] const vk::Device& GetHandle() const { return device; }

  [[nodiscard]] const QueueFamilyIndices& GetIndices() const { return family_indices; }

  [[nodiscard]] const vk::Queue& GetGraphicsQueue() const { return graphics_queue; }
  [[nodiscard]] const vk::Queue& GetPresentQueue() const { return present_queue; }

 private:
  QueueFamilyIndices family_indices;

  vk::Device device;

  vk::Queue graphics_queue;
  vk::Queue present_queue;
};

#endif  // LOGICALDEVICE_HPP
