#include "VulkanContext.hpp"

#include <vulkan/vulkan_core.h>

#include <Core/Log.hpp>
#include <Core/Memory.hpp>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan_handles.hpp>

VulkanContext::VulkanContext(const VulkanContextCreateOptions& options,
                             const std::shared_ptr<Window>& window) {
  vk::ApplicationInfo appInfo("rvulkan", VK_MAKE_VERSION(1, 0, 0), "rvulkan",  // NOLINT
                              VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_2);   // NOLINT

  vk::InstanceCreateInfo instanceInfo(vk::InstanceCreateFlags(), &appInfo, options.Layers,
                                      options.Extensions);
  instance = vk::createInstance(instanceInfo);

  // Init device
  physical_device = PhysicalDevice(instance);

  surface = Surface(instance, physical_device, window);

  device = LogicalDevice(physical_device, surface);

  CreateAllocator();
  RecreateSwapchain(0, 0);

  imageAvailableSemaphore = device.GetHandle().createSemaphore(vk::SemaphoreCreateInfo());
  renderingFinishedSemaphore = device.GetHandle().createSemaphore(vk::SemaphoreCreateInfo());

  vk::CommandPoolCreateInfo commandPoolInfo;
  commandPoolInfo
      .setQueueFamilyIndex(device.GetIndices().graphics_family.value())  // NOLINT
      .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
                vk::CommandPoolCreateFlagBits::eTransient);
  commandPool = device.GetHandle().createCommandPool(commandPoolInfo);
}

void VulkanContext::CreateAllocator() {
  VmaAllocatorCreateInfo allocator_info{};
  allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;  // NOLINT
  allocator_info.instance = instance;
  allocator_info.physicalDevice = physical_device.GetHandle();
  allocator_info.device = device.GetHandle();
  vmaCreateAllocator(&allocator_info, &allocator);
}

void VulkanContext::CleanupSwapchain() {}

void VulkanContext::RecreateSwapchain(uint32_t surfaceWidth, uint32_t surfaceHeight) {
  device.GetHandle().waitIdle();

  auto surfaceCapabilities =
      physical_device.GetHandle().getSurfaceCapabilitiesKHR(surface.GetHandle());
  surfaceExtent = vk::Extent2D(std::clamp(surfaceWidth, surfaceCapabilities.minImageExtent.width,
                                          surfaceCapabilities.maxImageExtent.width),
                               std::clamp(surfaceHeight, surfaceCapabilities.minImageExtent.height,
                                          surfaceCapabilities.maxImageExtent.height));

  vk::SwapchainCreateInfoKHR swapchainInfo;
  swapchainInfo.setSurface(surface.GetHandle())
      .setMinImageCount(surface.GetPresentImageCount())
      .setImageFormat(surface.GetFormat().format)
      .setImageColorSpace(surface.GetFormat().colorSpace)
      .setImageExtent(surfaceExtent)
      .setImageArrayLayers(1)
      .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
                     vk::ImageUsageFlagBits::eTransferDst)
      .setImageSharingMode(vk::SharingMode::eExclusive)
      .setPreTransform(surfaceCapabilities.currentTransform)
      .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
      .setPresentMode(surface.GetPresentMode())
      .setClipped(VK_TRUE)
      .setOldSwapchain(swapchain);

  swapchain = device.GetHandle().createSwapchainKHR(swapchainInfo);
  if (swapchainInfo.oldSwapchain)
    device.GetHandle().destroySwapchainKHR(swapchainInfo.oldSwapchain);

  swapchainImages = device.GetHandle().getSwapchainImagesKHR(swapchain);
  swapchainImageViews.clear();
  swapchainImageViews.reserve(swapchainImages.size());

  for (auto& swapchainImage : swapchainImages) {
    vk::ImageViewCreateInfo imageViewInfo;
    imageViewInfo.setImage(swapchainImage)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(surface.GetFormat().format)
        .setSubresourceRange(
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    swapchainImageViews.push_back(device.GetHandle().createImageView(imageViewInfo));
  }
}
