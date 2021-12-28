#include "Window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <set>

#include "Application.hpp"

Window::Window() {
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("rvulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900,
                            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
  if (!window) throw std::runtime_error("Failed to create window");
}

void Window::Init(const vk::Instance& instance, const vk::PhysicalDevice& physicalDevice) {
  createSurface(instance);
  createLogicalDevice(physicalDevice);
  createSwapchain(physicalDevice);
  createSyncObjects();
}

void Window::Update() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        eventCallback(true);
        break;
    }
  }
}

uint32_t Window::BeginFrame() {
  auto acquiredBuffer =
      device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphore, {});

  uint32_t acquiredImage = acquiredBuffer.value;

  return acquiredImage;
}

void Window::EndFrame(uint32_t acquiredImage) {
  // Submit
  std::array<vk::PipelineStageFlags, 1> waitStages = {
      vk::PipelineStageFlagBits::eColorAttachmentOutput};

  vk::SubmitInfo submitInfo(imageAvailableSemaphore, waitStages, {}, renderFinishedSemaphore);
  graphicsQueue.submit(submitInfo);

  // Present
  vk::PresentInfoKHR presentInfo(renderFinishedSemaphore, swapchain, acquiredImage);
  vk::Result result = presentQueue.presentKHR(presentInfo);
  if (result != vk::Result::eSuccess) throw std::runtime_error("Failed to present swapchain");

  presentQueue.waitIdle();

  currentFrame = (currentFrame + 1) % MAX_IMAGES_IN_FLIGHT;
}

void Window::createSwapchain(const vk::PhysicalDevice& physicalDevice) {
  SwapchainSupportDetails details = querySwapchainSupportDetails(physicalDevice);
  swapchainFormat = chooseSwapchainFormat(details.formats);
  swapchainPresentMode = chooseSwapchainPresentMode(details.presentModes);
  swapchainExtent = chooseSwapchainExtent(details.capabilities);

  uint32_t imageCount = details.capabilities.minImageCount + 1;
  if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
    imageCount = details.capabilities.maxImageCount;

  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
  std::vector<uint32_t> queueFamilyIndices = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()};

  vk::SharingMode sharingMode = vk::SharingMode::eConcurrent;
  if (indices.graphicsFamily == indices.presentFamily) {
    sharingMode = vk::SharingMode::eExclusive;
    queueFamilyIndices.clear();
  }

  vk::SwapchainCreateInfoKHR createInfo(
      {}, surface, imageCount, swapchainFormat.format, swapchainFormat.colorSpace, swapchainExtent,
      1, vk::ImageUsageFlagBits::eColorAttachment, sharingMode, queueFamilyIndices,
      details.capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
      swapchainPresentMode, true);

  swapchain = device.createSwapchainKHR(createInfo);
  swapchainImages = device.getSwapchainImagesKHR(swapchain);
}

void Window::createSwapchainImageViews() {
  swapchainImageViews.resize(swapchainImages.size());
  for (size_t i = 0; i < swapchainImages.size(); i++) {
    vk::ImageSubresourceRange sourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    vk::ImageViewCreateInfo info(vk::ImageViewCreateFlags(), swapchainImages[i],
                                 vk::ImageViewType::e2D, swapchainFormat.format,
                                 vk::ComponentMapping(), sourceRange);
    swapchainImageViews[i] = device.createImageView(info);
  }
}

void Window::createSyncObjects() {
  imagesInFlight.resize(swapchainImageViews.size(), VK_NULL_HANDLE);

  vk::SemaphoreCreateInfo semaphoreInfo((vk::SemaphoreCreateFlags()));
  vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);
  for (size_t i = 0; i < MAX_IMAGES_IN_FLIGHT; i++) {
    imageAvailableSemaphores.at(i) = device.createSemaphore(semaphoreInfo);
    renderFinishedSemaphores.at(i) = device.createSemaphore(semaphoreInfo);
    imageAvailableSemaphore = device.createSemaphore(semaphoreInfo);
    renderFinishedSemaphore = device.createSemaphore(semaphoreInfo);
    inFlightFences.at(i) = device.createFence(fenceInfo);
  }
}

void Window::createSurface(const vk::Instance& instance) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  bool success = SDL_Vulkan_CreateSurface(window, instance, &surface);
  if (!success) throw std::runtime_error("Failed to create window surface");
  this->surface = surface;
}

void Window::createLogicalDevice(const vk::PhysicalDevice& physicalDevice) {  // Device Queues
  auto indices = findQueueFamilies(physicalDevice);

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value()};

  float queuePrio = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    vk::DeviceQueueCreateInfo queueInfo(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePrio);
    queueCreateInfos.push_back(queueInfo);
  }

  vk::DeviceCreateInfo deviceInfo(vk::DeviceCreateFlags(), queueCreateInfos, {},
                                  REQUIRED_DEVICE_EXTENSIONS, {});
  device = physicalDevice.createDevice(deviceInfo);

  graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
  presentQueue = device.getQueue(indices.presentFamily.value(), 0);
}

QueueFamilyIndices Window::findQueueFamilies(const vk::PhysicalDevice& device) {
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

SwapchainSupportDetails Window::querySwapchainSupportDetails(
    const vk::PhysicalDevice& physicalDevice) {
  SwapchainSupportDetails details;

  details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
  details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
  details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

  return details;
}

vk::SurfaceFormatKHR Window::chooseSwapchainFormat(
    const std::vector<vk::SurfaceFormatKHR>& formats) {
  for (const auto& format : formats) {
    if (format.format == vk::Format::eB8G8R8A8Srgb &&
        format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
      return format;
  }
  return formats[0];
}

vk::PresentModeKHR Window::chooseSwapchainPresentMode(
    const std::vector<vk::PresentModeKHR>& presentModes) {
  for (const auto& mode : presentModes) {
    if (mode == vk::PresentModeKHR::eMailbox) return mode;
  }
  return presentModes[0];
}

vk::Extent2D Window::chooseSwapchainExtent(vk::SurfaceCapabilitiesKHR capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) return capabilities.currentExtent;

  int width = 0, height = 0;
  SDL_GetWindowSize(window, &width, &height);

  vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

  actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                  capabilities.maxImageExtent.width);
  actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                   capabilities.maxImageExtent.height);

  return actualExtent;
}
