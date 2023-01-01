#ifndef PHYSICALDEVICE_HPP
#define PHYSICALDEVICE_HPP

#include <optional>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

class Surface;

struct SwapchainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> present_modes;
};

struct QueueFamilyIndices {
  [[nodiscard]] bool IsComplete() const {
    return graphics_family.has_value() && present_family.has_value();
  }

  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;
};

class PhysicalDevice {
 public:
  PhysicalDevice() = default;
  explicit PhysicalDevice(const vk::Instance& instance, const Surface& surface);

  [[nodiscard]] const vk::PhysicalDevice& GetHandle() const { return physical_device; }

  static QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& physical_device,
                                              const Surface& surface);

 private:
  static SwapchainSupportDetails QuerySwapchainSupportDetails(const vk::PhysicalDevice& device,
                                                              const Surface& surface);
  static bool CheckExtensionSupport(const vk::PhysicalDevice& device);

  static bool IsDeviceSuitable(const vk::PhysicalDevice& device, const Surface& surface);

  vk::PhysicalDevice physical_device;
};

#endif  // PHYSICALDEVICE_HPP
