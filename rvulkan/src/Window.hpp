#pragma once
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <array>
#include <functional>
#include <optional>
#include <vulkan/vulkan.hpp>

using SDL_Window = struct SDL_Window;

class Window {
 public:
  Window();

  void Update();

  void SetEventCallback(std::function<void(bool)> cb) { eventCallback = cb; }

  std::vector<const char*> GetRequiredExtension();
  vk::SurfaceKHR GetSurface(const vk::Instance& instance);
  [[nodiscard]] SDL_Window* GetWindowHandle() const { return window; }

 private:
  SDL_Window* window = nullptr;

  std::function<void(bool)> eventCallback;

  uint64_t currentFrame = 0;
};

#endif  // WINDOW_HPP_
