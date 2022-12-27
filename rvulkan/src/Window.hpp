#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <array>
#include <events/event.hpp>
#include <functional>
#include <optional>
#include <utility>
#include <vulkan/vulkan.hpp>

using SDL_Window = struct SDL_Window;  // NOLINT

class Window {
 public:
  Window();

  void Update();

  void SetEventCallback(std::function<void(Event&)> cb) { event_callback = std::move(cb); }

  std::vector<const char*> GetRequiredExtension();
  vk::SurfaceKHR GetSurface(const vk::Instance& instance);
  [[nodiscard]] SDL_Window* GetWindowHandle() const { return window; }

 private:
  SDL_Window* window = nullptr;

  std::function<void(Event&)> event_callback;

  uint64_t currentFrame = 0;
};

#endif  // WINDOW_HPP_
