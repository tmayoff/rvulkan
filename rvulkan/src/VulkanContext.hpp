#pragma once
#ifndef VULKANCONTEXT_HPP_
#define VULKANCONTEXT_HPP_

#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

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
  VulkanContext() = default;
  explicit VulkanContext(const VulkanContextCreateOptions& options);

  void Init(vk::SurfaceKHR surface);

  [[nodiscard]] const vkb::Instance& GetInstance() const { return instance; }
  [[nodiscard]] vk::PhysicalDevice GetPhysicalDevice() const {
    return physical_device.physical_device;
  }
  [[nodiscard]] const vk::Device& GetDevice() const { return device; }
  [[nodiscard]] vk::SwapchainKHR GetSwapchain() const { return swapchain.swapchain; }
  [[nodiscard]] const vk::Extent2D& GetSurfaceExtent() const { return surfaceExtent; }
  [[nodiscard]] const vk::SurfaceFormatKHR& GetSurfaceFormat() const { return surfaceFormat; }
  [[nodiscard]] const vk::Queue& GetGraphicsQueue() const { return graphics_queue; }
  [[nodiscard]] const vk::Queue& GetPresentQueue() const { return present_queue; }
  [[nodiscard]] const vk::Semaphore& GetImageAvailableSemaphore() const {
    return imageAvailableSemaphore;
  }
  [[nodiscard]] const vk::Semaphore& GetRenderingFinishedSemaphore() const {
    return renderingFinishedSemaphore;
  }
  [[nodiscard]] const std::vector<vk::ImageView>& GetSwapchainImageViews() const {
    return swapchainImageViews;
  }

  [[nodiscard]] const VmaAllocator& GetAllocator() const { return allocator; }
  [[nodiscard]] const vk::CommandPool& GetCommandPool() const { return commandPool; }

 private:
  void pickPhysicalDevice(std::vector<vk::PhysicalDevice> devices);
  QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device);

  void CreateAllocator();
  void CreateSwapchain();
  void recreateSwapchain(uint32_t surfaceWidth, uint32_t surfaceHeight);

  vkb::Instance instance;

  vkb::PhysicalDevice physical_device;

  vk::Extent2D surfaceExtent;
  vk::PresentModeKHR surfacePresentMode;
  uint32_t presentImageCount = 1;
  vk::SurfaceFormatKHR surfaceFormat;

  vk::Device device;
  vkb::Device vkb_device;
  vk::Queue graphics_queue;
  vk::Queue present_queue;

  VmaAllocator allocator = {};

  vkb::Swapchain swapchain;
  std::vector<vk::Image> swapchainImages;
  std::vector<vk::ImageView> swapchainImageViews;

  vk::Semaphore imageAvailableSemaphore;
  vk::Semaphore renderingFinishedSemaphore;

  vk::CommandPool commandPool;
};

VulkanContext& GetCurrentVulkanContext();
void SetCurrentVulkanContext(VulkanContext&);

#endif  // VULKANCONTEXT_HPP_
