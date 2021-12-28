#pragma once
#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include <optional>
#include <vulkan/vulkan.hpp>

#include "Window.hpp"

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

const std::vector<const char*> REQUIRED_DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

class Application {
 public:
  Application();

  void Run();

 private:
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();

  QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);

  bool UsableDevice(vk::PhysicalDevice device);

  vk::Instance instance;
  vk::SurfaceKHR surface;
  vk::PhysicalDevice physicalDevice;

  // TODO Move to renderer
  vk::Device device;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;

  std::shared_ptr<Window> window;
};

#endif  // APPLICATION_HPP_
