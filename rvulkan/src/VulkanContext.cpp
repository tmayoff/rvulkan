#include "VulkanContext.hpp"

#include <Core/Log.hpp>
#include <iostream>
#include <set>
#include <stdexcept>

#include "VkBootstrap.h"

VulkanContext::VulkanContext(const VulkanContextCreateOptions& /*options*/) {
  vkb::InstanceBuilder builder;
  auto inst_ret = builder.set_app_name("rvulkan")
                      .request_validation_layers()
                      .use_default_debug_messenger()
                      .build();

  if (!inst_ret)
    throw std::runtime_error("Failed to create vulkan instance. Error: " +
                             inst_ret.error().message());

  instance = inst_ret.value();
}

void VulkanContext::Init(vk::SurfaceKHR surface) {
  // Init device
  vkb::PhysicalDeviceSelector selector{instance};
  auto phys_ret = selector.set_surface(surface)
                      .set_minimum_version(1, 2)
                      .require_dedicated_transfer_queue()
                      .select();
  if (!phys_ret)
    throw std::runtime_error("Failed to select Vulkan Physical Device. Error: " +
                             phys_ret.error().message());
  physical_device = phys_ret.value();

  vkb::DeviceBuilder device_builder{physical_device};
  auto dev_ret = device_builder.build();
  if (!dev_ret)
    throw std::runtime_error("Failed to create Vulkan Device. Error: " + dev_ret.error().message());

  device = dev_ret.value();
  vkb_device = dev_ret.value();

  CreateAllocator();
  CreateSwapchain();

  // Get Queues (potentially move this to the renderer)
  auto gq = vkb_device.get_queue(vkb::QueueType::graphics);
  if (!gq.has_value()) logger::fatal("Failed to get graphics queue");
  graphics_queue = gq.value();

  auto pq = vkb_device.get_queue(vkb::QueueType::present);
  if (!pq.has_value()) logger::fatal("Failed to get present queue");
  present_queue = pq.value();
}

void VulkanContext::CreateAllocator() {
  VmaAllocatorCreateInfo allocatorInfo{};
  allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
  allocatorInfo.instance = instance;
  allocatorInfo.physicalDevice = physical_device.physical_device;
  allocatorInfo.device = device;
  vmaCreateAllocator(&allocatorInfo, &allocator);
}

void VulkanContext::CreateSwapchain() {
  vkb::SwapchainBuilder swapchain_builder{vkb_device};
  auto swap_ret = swapchain_builder.set_old_swapchain(swapchain).build();
  if (!swap_ret) {
    std::cout << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
    throw std::runtime_error("Failed to create swapchain");
  }

  vkb::destroy_swapchain(swapchain);
  swapchain = swap_ret.value();
}

void VulkanContext::recreateSwapchain(uint32_t /*surfaceWidth*/, uint32_t /* surfaceHeight*/) {
  // device.waitIdle();

  // auto surfaceCapabilities = physical_device.getSurfaceCapabilitiesKHR(surface);
  // surfaceExtent = vk::Extent2D(std::clamp(surfaceWidth, surfaceCapabilities.minImageExtent.width,
  //                                         surfaceCapabilities.maxImageExtent.width),
  //                              std::clamp(surfaceHeight,
  //                              surfaceCapabilities.minImageExtent.height,
  //                                         surfaceCapabilities.maxImageExtent.height));

  // vk::SwapchainCreateInfoKHR swapchainInfo;
  // swapchainInfo.setSurface(surface)
  //     .setMinImageCount(presentImageCount)
  //     .setImageFormat(surfaceFormat.format)
  //     .setImageColorSpace(surfaceFormat.colorSpace)
  //     .setImageExtent(surfaceExtent)
  //     .setImageArrayLayers(1)
  //     .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
  //                    vk::ImageUsageFlagBits::eTransferDst)
  //     .setImageSharingMode(vk::SharingMode::eExclusive)
  //     .setPreTransform(surfaceCapabilities.currentTransform)
  //     .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
  //     .setPresentMode(surfacePresentMode)
  //     .setClipped(true)
  //     .setOldSwapchain(swapchain);

  // swapchain = device.createSwapchainKHR(swapchainInfo);
  // if (swapchainInfo.oldSwapchain) device.destroySwapchainKHR(swapchainInfo.oldSwapchain);

  // swapchainImages = device.getSwapchainImagesKHR(swapchain);
  // swapchainImageViews.clear();
  // swapchainImageViews.reserve(swapchainImages.size());

  // for (size_t i = 0; i < swapchainImages.size(); i++) {
  //   vk::ImageViewCreateInfo imageViewInfo;
  //   imageViewInfo.setImage(swapchainImages[i])
  //       .setViewType(vk::ImageViewType::e2D)
  //       .setFormat(surfaceFormat.format)
  //       .setSubresourceRange(
  //           vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
  //   swapchainImageViews.push_back(device.createImageView(imageViewInfo));
  // }
}

static VulkanContext* CurrentVulkanContext = nullptr;

VulkanContext& GetCurrentVulkanContext() {
  assert(CurrentVulkanContext != nullptr);
  return *CurrentVulkanContext;
}

void SetCurrentVulkanContext(VulkanContext& context) {
  CurrentVulkanContext = std::addressof(context);
}
