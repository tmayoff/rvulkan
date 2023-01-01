#include "VulkanContext.hpp"

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <Core/Log.hpp>
#include <Core/Memory.hpp>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;  // NOLINT

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) {
  return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VkBool32 DebugMessageFunc(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                          VkDebugUtilsMessageTypeFlagsEXT msg_types,
                          VkDebugUtilsMessengerCallbackDataEXT const* p_callback_data,
                          void* /*p_user_data*/) {
  auto severity =
      vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(msg_severity));
  auto type = vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(msg_types));

  logger::debug("{} {} {}", severity, type, p_callback_data->pMessage);

  return VK_FALSE;
}

VulkanContext::VulkanContext(const VulkanContextCreateOptions& options,
                             const std::shared_ptr<Window>& window) {
  logger::debug("Available vulkan layers: ");
  auto layer_props = vk::enumerateInstanceLayerProperties();
  for (const auto& l : layer_props) logger::debug("\t{}", l.layerName);

  std::vector<const char*> extensions(options.Extensions);
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  vk::ApplicationInfo appInfo("rvulkan", VK_MAKE_VERSION(1, 0, 0), "rvulkan",  // NOLINT
                              VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_3);   // NOLINT

  vk::InstanceCreateInfo instance_info(vk::InstanceCreateFlags(), &appInfo, options.Layers,
                                       extensions);
  instance = vk::createInstance(instance_info);

  // Create debug messenger
  pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
  if (pfnVkCreateDebugUtilsMessengerEXT == nullptr) {
    logger::fatal("GetInstanceProcAddr: Unable top find pfnVkCreateDebugUtilsMessengerEXT");
  }

  vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

  vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

  debug_utils_messenger =
      instance.createDebugUtilsMessengerEXT(vk::DebugUtilsMessengerCreateInfoEXT(
          {}, severity_flags, message_type_flags, &DebugMessageFunc));

  // Init device
  surface = Surface(instance, window);
  physical_device = PhysicalDevice(instance, surface);
  surface.Init(physical_device);

  device = LogicalDevice(physical_device, surface);

  CreateAllocator();

  vk::CommandPoolCreateInfo command_pool_info;
  command_pool_info
      .setQueueFamilyIndex(device.GetIndices().graphics_family.value())  // NOLINT
      .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
                vk::CommandPoolCreateFlagBits::eTransient);
  commandPool = device.GetHandle().createCommandPool(command_pool_info);
}

void VulkanContext::CreateAllocator() {
  VmaAllocatorCreateInfo allocator_info{};
  allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;  // NOLINT
  allocator_info.instance = instance;
  allocator_info.physicalDevice = physical_device.GetHandle();
  allocator_info.device = device.GetHandle();
  vmaCreateAllocator(&allocator_info, &allocator);
}
