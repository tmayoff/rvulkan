#ifndef LOGICALDEVICE_HPP
#define LOGICALDEVICE_HPP

#include <optional>

#include "PhysicalDevice.hpp"
#include "Surface.hpp"

struct QueueFamilyIndices {
  [[nodiscard]] bool IsComplete() const {
    return graphics_family.has_value() && present_family.has_value();
  }

  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;
};

class LogicalDevice {
 public:
  LogicalDevice() = default;
  explicit LogicalDevice(const PhysicalDevice& physical_device, const Surface& surface);

  [[nodiscard]] const vk::Device& GetHandle() const { return device; }

  [[nodiscard]] const QueueFamilyIndices& GetIndices() const { return family_indices; }

 private:
  static QueueFamilyIndices FindQueueFamilies(const PhysicalDevice& physical_device,
                                              const Surface& surface);

  QueueFamilyIndices family_indices;

  vk::Device device;

  vk::Queue graphics_queue;
  vk::Queue present_queue;
};

#endif  // LOGICALDEVICE_HPP
