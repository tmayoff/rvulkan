#include "Application.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <memory>
#include <set>
#include <stdexcept>

Application::Application() {
  window = std::make_shared<Window>();

  createInstance();
  pickPhysicalDevice();
  createLogicalDevice();
}

void Application::Run() {}

void Application::createInstance() {
  uint32_t extensionCount = 0;
  SDL_Vulkan_GetInstanceExtensions(window->GetWindowHandle(), &extensionCount, nullptr);
  std::vector<const char *> extensions(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(window->GetWindowHandle(), &extensionCount, extensions.data());

  const std::array<const char *, 1> layers = {"VK_LAYER_KHRONOS_validation"};

  vk::ApplicationInfo appInfo("rvulkan", VK_MAKE_VERSION(1, 0, 0), "rvulkan",
                              VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_2);

  vk::InstanceCreateInfo instanceInfo(vk::InstanceCreateFlags(), &appInfo, layers, extensions);
  instance = vk::createInstance(instanceInfo);
}

void Application::createSurface() {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (!SDL_Vulkan_CreateSurface(window->GetWindowHandle(), instance, &surface))
    throw std::runtime_error("Failed to create surface");

  this->surface = surface;
}

void Application::pickPhysicalDevice() {
  auto devices = instance.enumeratePhysicalDevices();

  for (const auto &d : devices) {
    if (UsableDevice(d)) {
      physicalDevice = d;
      break;
    }
  }

  // TODO verify we found a device
  if (!physicalDevice) throw std::runtime_error("Failed to find suitable GPU");
}

void Application::createLogicalDevice() {
  // Device Queues
  auto indices = FindQueueFamilies(physicalDevice);

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
}

QueueFamilyIndices Application::FindQueueFamilies(vk::PhysicalDevice device) {
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

bool Application::UsableDevice(vk::PhysicalDevice) {
  // TODO fill this out
  return true;
}
