#pragma once
#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

#include <optional>
#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  [[nodiscard]] auto IsComplete() const -> bool {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapchainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilites;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

class Context {
  const std::vector<const char*> requiredDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

 public:
  inline static std::shared_ptr<Context> Instance = nullptr;
  inline static auto Get() {
    if (Instance == nullptr) Instance = std::make_shared<Context>();
    return Instance;
  }

  Context();
  Context(Context const&) = delete;
  void operator=(Context const&) = delete;

  auto GetInstance() -> vk::Instance { return instance; }

  auto GetSurface() -> vk::SurfaceKHR { return surface; }
  auto GetDevice() -> vk::Device { return device; }
  auto GetPhysicalDevice() -> vk::PhysicalDevice { return physicalDevice; }
  auto GetGraphicsQueue() -> vk::Queue { return graphicsQueue; }
  auto GetPresentQueue() -> vk::Queue { return presentQueue; }

  auto FindQueueFamilies(const vk::PhysicalDevice& device) -> QueueFamilyIndices;
  auto QuerySwapchainSupportDetails(const vk::PhysicalDevice& device) -> SwapchainSupportDetails;

 private:
  void CreateInstance();
  void CreateSurface();
  void CreateLogicalDevice();
  void PickPhysicalDevice();
  auto IsDeviceSuitable(const vk::PhysicalDevice& device) -> bool;
  auto CheckExtensionSupport(const vk::PhysicalDevice& device) -> bool;

  vk::Instance instance;
  vk::PhysicalDevice physicalDevice;
  vk::SurfaceKHR surface;
  vk::Device device;
  vk::Queue presentQueue;
  vk::Queue graphicsQueue;
};

#endif  // CONTEXT_HPP_
