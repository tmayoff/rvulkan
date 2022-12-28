#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <memory>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "VulkanContext.hpp"

class Swapchain {
 public:
  explicit Swapchain(std::shared_ptr<VulkanContext> context, const vk::Extent2D& surface_extent);

  void RecreateSwapchain(const vk::Extent2D& surface_extent_);

  // Getters
  [[nodiscard]] const vk::SwapchainKHR& GetHandle() const { return swapchain; }
  [[nodiscard]] const std::vector<vk::ImageView>& GetImageViews() const {
    return swapchain_image_views;
  }

 private:
  [[nodiscard]] vk::Extent2D GetSurfaceExtent(const vk::SurfaceCapabilitiesKHR& surface_caps) const;
  void CreateSwapchain();
  void CreateImageViews();

  std::shared_ptr<VulkanContext> context;

  vk::Extent2D surface_extent;

  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> swapchain_images;
  std::vector<vk::ImageView> swapchain_image_views;
};

#endif  // SWAPCHAIN_HPP
