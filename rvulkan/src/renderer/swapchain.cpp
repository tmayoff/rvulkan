#include "swapchain.hpp"

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

Swapchain::Swapchain(std::shared_ptr<VulkanContext> context, const vk::Extent2D& surface_extent)
    : context(std::move(context)), surface_extent(surface_extent) {
  CreateSwapchain();
  CreateImageViews();
}

void Swapchain::RecreateSwapchain(const vk::Extent2D& surface_extent_) {
  surface_extent = surface_extent_;
  // TODO
}

vk::Extent2D Swapchain::GetSurfaceExtent(const vk::SurfaceCapabilitiesKHR& surface_caps) const {
  auto width = std::clamp(surface_extent.width, surface_caps.minImageExtent.width,
                          surface_caps.maxImageExtent.width);
  auto height = std::clamp(surface_extent.height, surface_caps.minImageExtent.height,
                           surface_caps.maxImageExtent.height);

  return {width, height};
}

void Swapchain::CreateSwapchain() {
  const auto physical_device = context->GetPhysicalDevice().GetHandle();
  const auto logical_device = context->GetLogicalDevice();
  const auto device = context->GetLogicalDevice().GetHandle();
  const auto surface = context->GetSurface();

  auto surface_caps = physical_device.getSurfaceCapabilitiesKHR(surface.GetHandle());
  auto surface_extent = GetSurfaceExtent(surface_caps);

  uint32_t image_count = surface_caps.minImageCount + 1;
  if (surface_caps.maxImageCount > 0 && image_count > surface_caps.maxImageCount)
    image_count = surface_caps.maxImageCount;

  auto indices = logical_device.GetIndices();
  std::vector<uint32_t> queue_family_indices = {indices.graphics_family.value(),
                                                indices.present_family.value()};

  vk::SharingMode sharing_mode = vk::SharingMode::eExclusive;
  if (logical_device.GetGraphicsQueue() != logical_device.GetPresentQueue()) {
    sharing_mode = vk::SharingMode::eConcurrent;
    queue_family_indices.clear();
  }

  vk::SwapchainCreateInfoKHR create_info(
      {}, surface.GetHandle(), image_count, surface.GetFormat().format,
      surface.GetFormat().colorSpace, surface_extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
      sharing_mode, queue_family_indices, surface_caps.currentTransform,
      vk::CompositeAlphaFlagBitsKHR::eOpaque, surface.GetPresentMode(), VK_TRUE);

  swapchain = device.createSwapchainKHR(create_info);
  swapchain_images = device.getSwapchainImagesKHR(swapchain);
}

void Swapchain::CreateImageViews() {
  const auto device = context->GetLogicalDevice().GetHandle();

  swapchain_image_views.clear();
  swapchain_image_views.reserve(swapchain_images.size());

  vk::ImageViewCreateInfo create_info(
      {}, {}, vk::ImageViewType::e2D, context->GetSurfaceFormat().format, vk::ComponentMapping{},
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

  for (const auto& image : swapchain_images) {
    create_info.setImage(image);
    swapchain_image_views.push_back(device.createImageView(create_info));
  }
}
