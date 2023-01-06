#ifndef VULKANCONTEXT_HPP_
#define VULKANCONTEXT_HPP_

#include <optional>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "core/memory.hpp"

class PhysicalDevice;
class LogicalDevice;
class Surface;
class Window;

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
  [[nodiscard]] const std::shared_ptr<PhysicalDevice>& GetPhysicalDevice() const {
    return physical_device;
  }
  [[nodiscard]] const std::shared_ptr<LogicalDevice>& GetLogicalDevice() const { return device; }
  [[nodiscard]] const std::shared_ptr<Surface>& GetSurface() const { return surface; }
  [[nodiscard]] const vk::SurfaceFormatKHR& GetSurfaceFormat() const;

  [[nodiscard]] const vk::DescriptorPool& GetDescriptorPool() const { return descriptor_pool; }
  [[nodiscard]] const vk::CommandPool& GetCommandPool() const { return command_pool; }

  [[nodiscard]] const VmaAllocator& GetAllocator() const { return allocator; }

 private:
  void CreateAllocator();
  void CreateDescriptorPool();

  vk::Instance instance;
  vk::DebugUtilsMessengerEXT debug_utils_messenger;

  std::shared_ptr<Surface> surface;

  std::shared_ptr<PhysicalDevice> physical_device;
  std::shared_ptr<LogicalDevice> device;

  VmaAllocator allocator{};

  vk::DescriptorPool descriptor_pool;
  vk::CommandPool command_pool;
};

#endif  // VULKANCONTEXT_HPP_
