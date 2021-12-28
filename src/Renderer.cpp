#include "Renderer.hpp"

#include <set>

#include "Application.hpp"

Renderer::Renderer(vk::PhysicalDevice physicalDevice) { createLogicalDevice(physicalDevice); }

uint32_t Renderer::BeginFrame() {
  auto acquiredBuffer = device.acquireNextImageKHR(swapchain, UINT64_MAX, {}, {});

  uint32_t acquiredImage = acquiredBuffer.value;

  return acquiredImage;
}

uint32_t Renderer::EndFrame() {
  // TODO present
}

void Renderer::createLogicalDevice(vk::PhysicalDevice physicalDevice) {
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

QueueFamilyIndices Renderer::FindQueueFamilies(vk::PhysicalDevice device) {
  QueueFamilyIndices indices;

  auto queueFamilies = device.getQueueFamilyProperties();
  int i = 0;
  for (auto q : queueFamilies) {
    if (q.queueFlags & vk::QueueFlagBits::eGraphics) indices.graphicsFamily = i;

    auto presentSupport = device.getSurfaceSupportKHR(i, Application::Get()->GetSurface());
    if (presentSupport) indices.presentFamily = i;

    if (indices.IsComplete()) break;

    i++;
  }

  return indices;
}
