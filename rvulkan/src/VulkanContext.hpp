#ifndef VULKANCONTEXT_HPP_
#define VULKANCONTEXT_HPP_

#include <VkBootstrap.h>

#include <optional>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/Surface.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "Window.hpp"

struct VulkanContextCreateOptions {
  std::vector<const char*> Extensions;
  std::vector<const char*> Layers;
};

class VulkanContext {
 public:
  VulkanContext() = default;
  explicit VulkanContext(const VulkanContextCreateOptions& options, const Window& window);

  [[nodiscard]] const vk::Instance& GetInstance() const { return instance; }
  [[nodiscard]] const PhysicalDevice& GetPhysicalDevice() const { return physical_device; }
  [[nodiscard]] const LogicalDevice& GetLogicalDevice() const { return device; }
  [[nodiscard]] vk::SwapchainKHR GetSwapchain() const { return swapchain; }
  [[nodiscard]] const vk::Extent2D& GetSurfaceExtent() const { return surfaceExtent; }
  [[nodiscard]] const vk::SurfaceFormatKHR& GetSurfaceFormat() const { return surface.GetFormat(); }
  [[nodiscard]] const vk::Semaphore& GetImageAvailableSemaphore() const {
    return imageAvailableSemaphore;
  }
  [[nodiscard]] const vk::Semaphore& GetRenderingFinishedSemaphore() const {
    return renderingFinishedSemaphore;
  }
  [[nodiscard]] const std::vector<vk::ImageView>& GetSwapchainImageViews() const {
    return swapchainImageViews;
  }

  [[nodiscard]] const vk::CommandPool& GetCommandPool() const { return commandPool; }

 private:
  void pickPhysicalDevice(std::vector<vk::PhysicalDevice> devices);

  void CreateAllocator();
  void RecreateSwapchain(uint32_t surfaceWidth, uint32_t surfaceHeight);

  vk::Instance instance;

  Surface surface;

  PhysicalDevice physical_device;
  LogicalDevice device;

  vk::Extent2D surfaceExtent;

  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> swapchainImages;
  std::vector<vk::ImageView> swapchainImageViews;

  vk::Semaphore imageAvailableSemaphore;
  vk::Semaphore renderingFinishedSemaphore;

  vk::CommandPool commandPool;
};

#endif  // VULKANCONTEXT_HPP_
