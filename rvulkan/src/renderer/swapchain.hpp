#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <RenderPass.hpp>
#include <memory>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "VulkanContext.hpp"

class Swapchain {
 public:
  Swapchain() = default;
  explicit Swapchain(std::shared_ptr<VulkanContext> context,
                     std::shared_ptr<RenderPass> render_pass, const vk::Extent2D& surface_extent);

  void RecreateSwapchain(const vk::Extent2D& surface_extent_);

  // Getters
  [[nodiscard]] const vk::SwapchainKHR& GetHandle() const { return swapchain; }
  [[nodiscard]] const std::vector<vk::ImageView>& GetImageViews() const { return image_views; }

  [[nodiscard]] const std::vector<vk::Framebuffer>& GetFramebuffers() const { return framebuffers; }

 private:
  [[nodiscard]] vk::Extent2D GetSurfaceExtent(const vk::SurfaceCapabilitiesKHR& surface_caps) const;

  void CleanupSwapchain();
  void CreateSwapchain();
  void CreateFramebuffers();
  void CreateImageViews();

  std::shared_ptr<VulkanContext> context;
  std::shared_ptr<RenderPass> render_pass;

  vk::Extent2D surface_extent;

  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> images;
  std::vector<vk::ImageView> image_views;
  std::vector<vk::Framebuffer> framebuffers;
};

#endif  // SWAPCHAIN_HPP
