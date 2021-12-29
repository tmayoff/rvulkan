#pragma once
#ifndef VULKANCONTEXT_HPP_
#define VULKANCONTEXT_HPP_

#include <optional>
#include <vulkan/vulkan.hpp>

struct VmaAllocator_T;
using VmaAllocator = VmaAllocator_T*;

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

  const vk::Instance& GetInstance() const { return instance; }
  const vk::SurfaceKHR& GetSurface() const { return surface; }
  const vk::PhysicalDevice& GetPhysicalDevice() const { return physicalDevice; }
  const vk::Device& GetDevice() const { return device; }
  const vk::SwapchainKHR& GetSwapchain() const { return swapchain; }
  const vk::Queue& GetGraphicsQueue() const { return graphicsQueue; }
  const vk::Queue& GetPresentQueue() const { return presentQueue; }
  const vk::Semaphore& GetImageAvailableSemaphore() const { return imageAvailableSemaphore; }
  const vk::Semaphore& GetRenderingFinishedSemaphore() const { return renderingFinishedSemaphore; }

  const VmaAllocator& GetAllocator() const { return allocator; }
  const vk::CommandPool& GetCommandPool() const { return commandPool; }

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

  VmaAllocator allocator = {};

  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> swapchainImages;
  std::vector<vk::ImageView> swapchainImageViews;

  vk::Semaphore imageAvailableSemaphore;
  vk::Semaphore renderingFinishedSemaphore;

  vk::CommandPool commandPool;
};

VulkanContext& GetCurrentVulkanContext();
void SetCurrentVulkanContext(VulkanContext&);

#endif  // VULKANCONTEXT_HPP_
