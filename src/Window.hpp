#pragma once
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <array>
#include <functional>
#include <optional>
#include <vulkan/vulkan.hpp>

typedef struct SDL_Window SDL_Window;

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapchainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

class Window {
  static const uint32_t MAX_IMAGES_IN_FLIGHT = 2;

 public:
  /// @brief Initialize SDL window
  ///
  Window();

  /// @brief Initialize the rest of the vulkan swapchain
  ///
  /// @param instance
  /// @param device
  void Init(const vk::Instance& instance, const vk::PhysicalDevice& device);

  void Update();

  uint32_t BeginFrame();
  void EndFrame(uint32_t acquiredImage);

  void SetEventCallback(std::function<void(bool)> cb) { eventCallback = cb; }

  SDL_Window* GetWindowHandle() const { return window; }

 private:
  void createSurface(const vk::Instance& instance);
  void createLogicalDevice(const vk::PhysicalDevice& physicalDevice);
  void createSwapchain(const vk::PhysicalDevice& physicalDevice);
  void createSwapchainImageViews();

  void createSyncObjects();

  QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device);
  SwapchainSupportDetails querySwapchainSupportDetails(const vk::PhysicalDevice& device);
  vk::SurfaceFormatKHR chooseSwapchainFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
  vk::PresentModeKHR chooseSwapchainPresentMode(
      const std::vector<vk::PresentModeKHR>& presentModes);
  vk::Extent2D chooseSwapchainExtent(vk::SurfaceCapabilitiesKHR capabilities);

  SDL_Window* window = nullptr;

  vk::SurfaceKHR surface;
  vk::Device device;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;

  vk::SwapchainKHR swapchain;
  vk::SurfaceFormatKHR swapchainFormat;
  vk::PresentModeKHR swapchainPresentMode;
  vk::Extent2D swapchainExtent;
  std::vector<vk::Image> swapchainImages;
  std::vector<vk::ImageView> swapchainImageViews;

  std::vector<vk::Fence> imagesInFlight;
  std::array<vk::Fence, MAX_IMAGES_IN_FLIGHT> inFlightFences;
  std::array<vk::Semaphore, MAX_IMAGES_IN_FLIGHT> imageAvailableSemaphores;
  std::array<vk::Semaphore, MAX_IMAGES_IN_FLIGHT> renderFinishedSemaphores;
  vk::Semaphore imageAvailableSemaphore;
  vk::Semaphore renderFinishedSemaphore;

  std::function<void(bool)> eventCallback;

  uint64_t currentFrame = 0;
};

#endif  // WINDOW_HPP_
