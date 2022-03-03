#include "Context.hpp"

#include <SDL2/SDL_vulkan.h>

#include <cassert>
#include <set>
#include <string>

#include "Application.hpp"

Context::Context() {
  CreateInstance();
  CreateSurface();
  PickPhysicalDevice();
  CreateLogicalDevice();
}

void Context::CreateInstance() {
  uint32_t extensionCount = 0;
  SDL_Vulkan_GetInstanceExtensions(Application::Get().GetWindow(), &extensionCount, nullptr);
  std::vector<const char *> extensions(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(Application::Get().GetWindow(), &extensionCount,
                                   extensions.data());

  // Layers
  const std::array<const char *, 1> layers = {"VK_LAYER_KHRONOS_validation"};

  // Create Instance
  vk::ApplicationInfo appInfo("dvulkan", 1, "dvulkan", 1, VK_API_VERSION_1_2);
  vk::InstanceCreateInfo createInfo({}, &appInfo, layers, extensions);
  instance = vk::createInstance(createInfo);
}

void Context::CreateSurface() {
  auto window = Application::Get().GetWindow();

  // Surface creation
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (SDL_Vulkan_CreateSurface(window, instance, &surface) == 0U)
    throw std::runtime_error("Failed to create surface: " + std::string(SDL_GetError()));
  this->surface = surface;
}

void Context::CreateLogicalDevice() {
  // Create Logical Device
  auto indices = FindQueueFamilies(physicalDevice);

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  float queuePrio = 1.0F;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    vk::DeviceQueueCreateInfo queueCreateInfo({}, queueFamily, 1, &queuePrio);
    queueCreateInfos.push_back(queueCreateInfo);
  }

  vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), queueCreateInfos, {},
                                  requiredDeviceExtensions, {});
  device = physicalDevice.createDevice(createInfo);

  graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
  presentQueue = device.getQueue(indices.presentFamily.value(), 0);
}

void Context::PickPhysicalDevice() {
  // Picking physical device
  auto devices = instance.enumeratePhysicalDevices();

  for (const auto &d : devices) {
    if (IsDeviceSuitable(d)) {
      physicalDevice = d;
      break;
    }
  }
}

auto Context::IsDeviceSuitable(const vk::PhysicalDevice &device) -> bool {
  auto indices = FindQueueFamilies(device);

  bool extensionsSupported = CheckExtensionSupport(device);

  bool swapchainAdequate = false;
  if (extensionsSupported) {
    auto details = QuerySwapchainSupportDetails(device);
    swapchainAdequate = !details.formats.empty() && !details.presentModes.empty();
  }

  return indices.IsComplete() && extensionsSupported && swapchainAdequate;
}

auto Context::FindQueueFamilies(const vk::PhysicalDevice &device) -> QueueFamilyIndices {
  QueueFamilyIndices indices{};

  auto queueFamilies = device.getQueueFamilyProperties();

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) indices.graphicsFamily = i;
    auto presentSupport = device.getSurfaceSupportKHR(i, surface);

    if (presentSupport != 0) indices.presentFamily = i;
    if (indices.IsComplete()) break;

    i++;
  }

  return indices;
}

auto Context::CheckExtensionSupport(const vk::PhysicalDevice &device) -> bool {
  auto availableExtensions = device.enumerateDeviceExtensionProperties();

  std::set<std::string> deviceExtensions(requiredDeviceExtensions.begin(),
                                         requiredDeviceExtensions.end());

  for (const auto &e : availableExtensions) deviceExtensions.erase(e.extensionName);
  return deviceExtensions.empty();
}

auto Context::QuerySwapchainSupportDetails(const vk::PhysicalDevice &device)
    -> SwapchainSupportDetails {
  SwapchainSupportDetails details;

  details.capabilites = device.getSurfaceCapabilitiesKHR(surface);

  details.formats = device.getSurfaceFormatsKHR(surface);
  details.presentModes = device.getSurfacePresentModesKHR(surface);

  return details;
}
