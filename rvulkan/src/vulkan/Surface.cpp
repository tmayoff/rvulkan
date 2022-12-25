#include "Surface.hpp"

#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan_enums.hpp>

Surface::Surface(const vk::Instance& instance, const PhysicalDevice& physical_device,
                 const Window& window)
    : present_mode(vk::PresentModeKHR::eImmediate) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  SDL_Vulkan_CreateSurface(window.GetWindowHandle(), instance, &surface);

  this->surface = surface;

  auto present_modes = physical_device.GetHandle().getSurfacePresentModesKHR(surface);
  auto surface_caps = physical_device.GetHandle().getSurfaceCapabilitiesKHR(surface);
  auto surface_formats = physical_device.GetHandle().getSurfaceFormatsKHR(surface);

  // Get prefered present mode
  if (std::find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eMailbox) !=
      present_modes.end()) {
    present_mode = vk::PresentModeKHR::eMailbox;
  }

  present_image_count = surface_caps.minImageCount;

  format = surface_formats.front();
  for (const auto& format : surface_formats) {
    if (format.format == vk::Format::eB8G8R8A8Srgb &&
        format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
      this->format = format;
  }
}
