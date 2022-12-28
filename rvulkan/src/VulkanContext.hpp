#ifndef VULKANCONTEXT_HPP_
#define VULKANCONTEXT_HPP_

#include <Core/Memory.hpp>
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
  explicit VulkanContext(const VulkanContextCreateOptions& options,
                         const std::shared_ptr<Window>& window);

  [[nodiscard]] const vk::Instance& GetInstance() const { return instance; }
  [[nodiscard]] const PhysicalDevice& GetPhysicalDevice() const { return physical_device; }
  [[nodiscard]] const LogicalDevice& GetLogicalDevice() const { return device; }
  [[nodiscard]] const Surface& GetSurface() const { return surface; }
  [[nodiscard]] const vk::SurfaceFormatKHR& GetSurfaceFormat() const { return surface.GetFormat(); }

  [[nodiscard]] const vk::CommandPool& GetCommandPool() const { return commandPool; }

  [[nodiscard]] const VmaAllocator& GetAllocator() const { return allocator; }

 private:
  void CreateAllocator();

  vk::Instance instance;

  Surface surface;

  PhysicalDevice physical_device;
  LogicalDevice device;

  VmaAllocator allocator{};

  vk::CommandPool commandPool;
};

#endif  // VULKANCONTEXT_HPP_
