#pragma once
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <array>
#include <functional>
#include <optional>
#include <vulkan/vulkan.hpp>

typedef struct SDL_Window SDL_Window;

class Window {
  static constexpr std::array<const char*, 1> REQUIRED_DEVICE_EXTENSIONS = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  static const uint32_t MAX_IMAGES_IN_FLIGHT = 2;

 public:
  /// @brief Initialize SDL window
  ///
  Window();

  void Update();

  void SetEventCallback(std::function<void(bool)> cb) { eventCallback = cb; }

  std::vector<const char*> GetRequiredExtension();
  SDL_Window* GetWindowHandle() const { return window; }

 private:
  SDL_Window* window = nullptr;

  std::function<void(bool)> eventCallback;

  uint64_t currentFrame = 0;
};

#endif  // WINDOW_HPP_
