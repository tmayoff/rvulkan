
#include <iostream>
#include <rvulkan/core/log.hpp>
#include <rvulkan/core/memory.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <set>
#include <stdexcept>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/Surface.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;    // NOLINT
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;  // NOLINT

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) {
  return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                const VkAllocationCallbacks* pAllocator) {
  return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VkBool32 DebugMessageFunc(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                          VkDebugUtilsMessageTypeFlagsEXT msg_types,
                          VkDebugUtilsMessengerCallbackDataEXT const* p_callback_data,
                          void* /*p_user_data*/) {
  auto severity =
      vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(msg_severity));
  auto type = vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(msg_types));

  switch (msg_severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      logger::trace("{} {}", type, p_callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      logger::info("{} {}", type, p_callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      logger::warning("{} {}", type, p_callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      logger::error("{} {}", type, p_callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
      logger::fatal("{} {}", type, p_callback_data->pMessage);
      break;
  }

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
    logger::fatal("GetInstanceProcAddr: Unable top find vkCreateDebugUtilsMessengerEXT");
  }

  pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
  if (pfnVkDestroyDebugUtilsMessengerEXT == nullptr)
    logger::fatal("GetInstanceProcAddr: Unable top find vkDestroyDebugUtilsMessengerEXT");

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
  surface = std::make_shared<Surface>(instance, window);
  physical_device = std::make_shared<PhysicalDevice>(instance, surface);
  surface->Init(physical_device);

  device = std::make_shared<LogicalDevice>(physical_device, surface);

  CreateAllocator();
  CreateDescriptorPool();

  vk::CommandPoolCreateInfo command_pool_info;
  command_pool_info
      .setQueueFamilyIndex(device->GetIndices().graphics_family.value())  // NOLINT
      .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
                vk::CommandPoolCreateFlagBits::eTransient);
  command_pool = device->GetHandle().createCommandPool(command_pool_info);
}

VulkanContext::~VulkanContext() {
  device->GetHandle().destroyCommandPool(command_pool);
  device->GetHandle().destroyDescriptorPool(descriptor_pool);
  vmaDestroyAllocator(allocator);

  instance.destroySurfaceKHR(surface->GetHandle());
  device->GetHandle().destroy();

  instance.destroyDebugUtilsMessengerEXT(debug_utils_messenger);
  instance.destroy();
}

const vk::SurfaceFormatKHR& VulkanContext::GetSurfaceFormat() const { return surface->GetFormat(); }

void VulkanContext::CreateAllocator() {
  VmaAllocatorCreateInfo allocator_info{};
  allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;  // NOLINT
  allocator_info.instance = instance;
  allocator_info.physicalDevice = physical_device->GetHandle();
  allocator_info.device = device->GetHandle();
  vmaCreateAllocator(&allocator_info, &allocator);
}

void VulkanContext::CreateDescriptorPool() {
  std::array pool_sizes = {
      vk::DescriptorPoolSize{vk::DescriptorType::eSampler, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, 1000},
      vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, 1000},
  };

  vk::DescriptorPoolCreateInfo create_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                           1000 * pool_sizes.size(), pool_sizes);

  descriptor_pool = device->GetHandle().createDescriptorPool(create_info);
}
