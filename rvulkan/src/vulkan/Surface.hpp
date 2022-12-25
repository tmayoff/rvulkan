#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <Window.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "vulkan/PhysicalDevice.hpp"

class Surface {
 public:
  Surface() = default;
  Surface(const vk::Instance& instance, const PhysicalDevice& physical_device,
          const Window& window);

  [[nodiscard]] uint32_t GetPresentImageCount() const { return present_image_count; }
  [[nodiscard]] const vk::PresentModeKHR& GetPresentMode() const { return present_mode; }
  [[nodiscard]] const vk::SurfaceFormatKHR& GetFormat() const { return format; }

  [[nodiscard]] const vk::SurfaceKHR& GetHandle() const { return surface; }

 private:
  vk::SurfaceKHR surface;

  vk::PresentModeKHR present_mode;

  vk::SurfaceFormatKHR format;

  uint32_t present_image_count;
};

#endif  // SURFACE_HPP
