#include "VulkanContext.hpp"

#include <set>

VulkanContext::VulkanContext(const VulkanContextCreateOptions& options) {
  vk::ApplicationInfo appInfo("rvulkan", VK_MAKE_VERSION(1, 0, 0), "rvulkan",
                              VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_2);

  vk::InstanceCreateInfo instanceInfo(vk::InstanceCreateFlags(), &appInfo, options.Layers,
                                      options.Extensions);
  instance = vk::createInstance(instanceInfo);
}

void VulkanContext::Init(vk::SurfaceKHR surface) {
  this->surface = surface;

  pickPhysicalDevice(instance.enumeratePhysicalDevices());

  // Present Modes
  auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
  auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
  auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

  // Pick best present mode available
  surfacePresentMode = vk::PresentModeKHR::eImmediate;
  if (std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eMailbox) !=
      presentModes.end())
    surfacePresentMode = vk::PresentModeKHR::eMailbox;

  presentImageCount = std::max(surfaceCapabilities.maxImageCount, (uint32_t)1);

  // Pick surface format
  surfaceFormat = surfaceFormats.front();
  for (const auto& format : surfaceFormats) {
    if (format.format == vk::Format::eB8G8R8A8Srgb &&
        format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
      surfaceFormat = format;
  }

  // Create logical device
  auto indices = findQueueFamilies(physicalDevice);

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value()};

  float queuePrio = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    vk::DeviceQueueCreateInfo queueInfo(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePrio);
    queueCreateInfos.push_back(queueInfo);
  }

  std::vector<const char*> deviceExtensions;
  deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  vk::DeviceCreateInfo deviceInfo(vk::DeviceCreateFlags(), queueCreateInfos, {}, deviceExtensions,
                                  {});
  device = physicalDevice.createDevice(deviceInfo);

  graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
  presentQueue = device.getQueue(indices.presentFamily.value(), 0);

  // TODO Debug setup

  // TODO Vulkan Memory allocator

  recreateSwapchain(surfaceExtent.width, surfaceExtent.width);

  // TODO Create Sync

  vk::CommandPoolCreateInfo commandPoolInfo;
  commandPoolInfo.setQueueFamilyIndex(indices.graphicsFamily.value())
      .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
                vk::CommandPoolCreateFlagBits::eTransient);
  commandPool = device.createCommandPool(commandPoolInfo);

  // TODO Create Command Buffers
}

void VulkanContext::pickPhysicalDevice(std::vector<vk::PhysicalDevice> devices) {
  // TODO put some thought into this
  for (auto d : devices) {
    physicalDevice = d;
    break;
  }
}

QueueFamilyIndices VulkanContext::findQueueFamilies(const vk::PhysicalDevice& device) {
  QueueFamilyIndices indices;

  auto queueFamilies = device.getQueueFamilyProperties();
  int i = 0;
  for (auto q : queueFamilies) {
    if (q.queueFlags & vk::QueueFlagBits::eGraphics) indices.graphicsFamily = i;

    auto presentSupport = device.getSurfaceSupportKHR(i, surface);
    if (presentSupport) indices.presentFamily = i;

    if (indices.IsComplete()) break;

    i++;
  }

  return indices;
}

void VulkanContext::recreateSwapchain(uint32_t surfaceWidth, uint32_t surfaceHeight) {
  device.waitIdle();

  auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
  surfaceExtent = vk::Extent2D(std::clamp(surfaceWidth, surfaceCapabilities.minImageExtent.width,
                                          surfaceCapabilities.maxImageExtent.width),
                               std::clamp(surfaceHeight, surfaceCapabilities.minImageExtent.height,
                                          surfaceCapabilities.maxImageExtent.height));

  vk::SwapchainCreateInfoKHR swapchainInfo;
  swapchainInfo.setSurface(surface)
      .setMinImageCount(presentImageCount)
      .setImageFormat(surfaceFormat.format)
      .setImageColorSpace(surfaceFormat.colorSpace)
      .setImageExtent(surfaceExtent)
      .setImageArrayLayers(1)
      .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
                     vk::ImageUsageFlagBits::eTransferDst)
      .setImageSharingMode(vk::SharingMode::eExclusive)
      .setPreTransform(surfaceCapabilities.currentTransform)
      .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
      .setPresentMode(surfacePresentMode)
      .setClipped(true)
      .setOldSwapchain(swapchain);

  swapchain = device.createSwapchainKHR(swapchainInfo);
  if (swapchainInfo.oldSwapchain) device.destroySwapchainKHR(swapchainInfo.oldSwapchain);

  swapchainImages = device.getSwapchainImagesKHR(swapchain);
  swapchainImageViews.clear();

  for (size_t i = 0; i < swapchainImages.size(); i++) {
    vk::ImageViewCreateInfo imageViewInfo;
    imageViewInfo.setImage(swapchainImages[i])
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(surfaceFormat.format)
        .setSubresourceRange(
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    swapchainImageViews.at(i) = device.createImageView(imageViewInfo);
  }
}
