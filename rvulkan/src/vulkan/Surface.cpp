#include "Surface.hpp"

#include <SDL2/SDL_vulkan.h>

#include <core/log.hpp>
#include <vulkan/vulkan_enums.hpp>

Surface::Surface(const vk::Instance& instance, const std::shared_ptr<Window>& window)
    : present_mode(vk::PresentModeKHR::eFifo) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  SDL_Vulkan_CreateSurface(window->GetWindowHandle(), instance, &surface);

  this->surface = surface;
}

void Surface::Init(const PhysicalDevice& physical_device) {
  auto present_modes = physical_device.GetHandle().getSurfacePresentModesKHR(surface);
  auto surface_caps = physical_device.GetHandle().getSurfaceCapabilitiesKHR(surface);
  auto surface_formats = physical_device.GetHandle().getSurfaceFormatsKHR(surface);

  // Get prefered present mode
  if (std::find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eMailbox) !=
      present_modes.end()) {
    logger::info("Using Prefered presentation mode: Mailbox");
    present_mode = vk::PresentModeKHR::eMailbox;
  }

  logger::debug("Present Mode {}", vk::to_string(present_mode));

  present_image_count = surface_caps.maxImageCount;

  format = surface_formats.front();
  for (const auto& f : surface_formats) {
    if (f.format == vk::Format::eB8G8R8A8Srgb && f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
      format = f;
  }
}
