#pragma once
#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <optional>
#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

const std::vector<const char*> REQUIRED_DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

class Renderer {
 public:
  Renderer(vk::PhysicalDevice physicalDevice);

  uint32_t BeginFrame();
  uint32_t EndFrame();

 private:
  void createLogicalDevice(vk::PhysicalDevice physicalDevice);
  QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);

  vk::Device device;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;

  vk::SwapchainKHR swapchain;
};

#endif  // RENDERER_HPP_
