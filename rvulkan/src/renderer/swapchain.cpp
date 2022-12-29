#include "swapchain.hpp"

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

Swapchain::Swapchain(std::shared_ptr<VulkanContext> vulkan_context_,
                     std::shared_ptr<RenderPass> render_pass_, const vk::Extent2D& surface_extent)
    : context(std::move(vulkan_context_)),
      render_pass(std::move(render_pass_)),
      surface_extent(surface_extent) {
  CreateSwapchain();
  CreateImageViews();
  CreateFramebuffers();
}

void Swapchain::RecreateSwapchain(const vk::Extent2D& surface_extent_) {
  surface_extent = surface_extent_;

  context->GetLogicalDevice().GetHandle().waitIdle();

  CleanupSwapchain();

  CreateSwapchain();
  CreateImageViews();
  CreateFramebuffers();
}

vk::Extent2D Swapchain::GetSurfaceExtent(const vk::SurfaceCapabilitiesKHR& surface_caps) const {
  auto width = std::clamp(surface_extent.width, surface_caps.minImageExtent.width,
                          surface_caps.maxImageExtent.width);
  auto height = std::clamp(surface_extent.height, surface_caps.minImageExtent.height,
                           surface_caps.maxImageExtent.height);

  return {width, height};
}

void Swapchain::CleanupSwapchain() {
  const auto device = context->GetLogicalDevice().GetHandle();

  for (const auto& framebuffer : framebuffers) device.destroyFramebuffer(framebuffer);
  for (const auto& image_view : image_views) device.destroyImageView(image_view);

  device.destroySwapchainKHR(swapchain);
}

void Swapchain::CreateSwapchain() {
  const auto physical_device = context->GetPhysicalDevice().GetHandle();
  const auto logical_device = context->GetLogicalDevice();
  const auto device = context->GetLogicalDevice().GetHandle();
  const auto surface = context->GetSurface();

  auto surface_caps = physical_device.getSurfaceCapabilitiesKHR(surface.GetHandle());
  auto surface_extent = GetSurfaceExtent(surface_caps);

  uint32_t image_count = surface_caps.minImageCount + 1;
  if (surface_caps.maxImageCount > 0) image_count = surface_caps.maxImageCount;

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
  images = device.getSwapchainImagesKHR(swapchain);
}

void Swapchain::CreateFramebuffers() {
  framebuffers.resize(image_views.size());

  for (size_t i = 0; i < image_views.size(); i++) {
    std::array attachments = {image_views.at(i)};

    vk::FramebufferCreateInfo framebufferInfo({}, render_pass->GetHandle(), attachments,
                                              surface_extent.width, surface_extent.height, 1);
    framebuffers.at(i) = context->GetLogicalDevice().GetHandle().createFramebuffer(framebufferInfo);
  }
}

void Swapchain::CreateImageViews() {
  const auto device = context->GetLogicalDevice().GetHandle();

  image_views.clear();
  image_views.reserve(images.size());

  vk::ImageViewCreateInfo create_info(
      {}, {}, vk::ImageViewType::e2D, context->GetSurfaceFormat().format, vk::ComponentMapping{},
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

  for (const auto& image : images) {
    create_info.setImage(image);
    image_views.push_back(device.createImageView(create_info));
  }
}
