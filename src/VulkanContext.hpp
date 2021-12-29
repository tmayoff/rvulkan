#pragma once
#ifndef VULKANCONTEXT_HPP_
#define VULKANCONTEXT_HPP_

#include <optional>
#include <vulkan/vulkan.hpp>

struct VulkanContextCreateOptions {
  std::vector<const char*> Extensions;
  std::vector<const char*> Layers;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class VulkanContext {
 public:
  VulkanContext(const VulkanContextCreateOptions& options);

  void Init(vk::SurfaceKHR surface);

  const vk::Instance& GetInstance() { return instance; }
  const vk::SurfaceKHR& GetSurface() { return surface; }
  const vk::PhysicalDevice& GetPhysicalDevice() { return physicalDevice; }

 private:
  void pickPhysicalDevice(std::vector<vk::PhysicalDevice> devices);
  QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device);

  void recreateSwapchain(uint32_t surfaceWidth, uint32_t surfaceHeight);

  vk::Instance instance;
  vk::PhysicalDevice physicalDevice;

  vk::SurfaceKHR surface;
  vk::Extent2D surfaceExtent;
  vk::PresentModeKHR surfacePresentMode;
  uint32_t presentImageCount = 1;
  vk::SurfaceFormatKHR surfaceFormat;

  vk::Device device;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;

  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> swapchainImages;
  std::vector<vk::ImageView> swapchainImageViews;

  vk::CommandPool commandPool;
};

#endif  // VULKANCONTEXT_HPP_
