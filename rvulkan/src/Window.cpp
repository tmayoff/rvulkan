#include "Window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <debug/profiler.hpp>
#include <rvulkan/core/log.hpp>
#include <rvulkan/events/mouse_events.hpp>
#include <rvulkan/events/window_events.hpp>
#include <set>

#include "SDL_events.h"
#include "SDL_mouse.h"
#include "SDL_video.h"
#include "rvulkan/events/mouse_codes.hpp"

Window::Window()
    : window(SDL_CreateWindow("rvulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)) {
  SDL_Init(SDL_INIT_VIDEO);

  if (window == nullptr) throw std::runtime_error("Failed to create window");
}

void Window::Update() {
  ZoneScoped;  // NOLINT

  SDL_Event event;

  while (SDL_PollEvent(&event) == 1) {
    switch (event.type) {
      case SDL_QUIT: {
        WindowCloseEvent e;
        event_callback(e);
        break;
      }

      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
          case SDL_WINDOWEVENT_RESIZED: {
            WindowResizeEvent e({event.window.data1, event.window.data2});
            event_callback(e);
            break;
          }
        }
        break;
      }
      case SDL_MOUSEMOTION: {
        // auto deltaX = 0;
        // auto deltaY = 0;

        // SDL_GetRelativeMouseState(&deltaX, &deltaY);
        MouseMoveEvent e({event.motion.x, event.motion.y});

        break;
      }
      case SDL_MOUSEBUTTONUP: {
        MouseButton button = MouseButton::Button0;
        if (event.button.button == SDL_BUTTON_LEFT) button = MouseButton::ButtonLeft;
        if (event.button.button == SDL_BUTTON_MIDDLE) button = MouseButton::ButtonMiddle;
        if (event.button.button == SDL_BUTTON_RIGHT) button = MouseButton::ButtonRight;

        MouseButtonReleasedEvent e(static_cast<MouseButton>(button));
        event_callback(e);
        break;
      }
      case SDL_MOUSEBUTTONDOWN: {
        MouseButton button = MouseButton::Button0;
        if (event.button.button == SDL_BUTTON_LEFT) button = MouseButton::ButtonLeft;
        if (event.button.button == SDL_BUTTON_MIDDLE) button = MouseButton::ButtonMiddle;
        if (event.button.button == SDL_BUTTON_RIGHT) button = MouseButton::ButtonRight;

        MouseButtonPressedEvent e(static_cast<MouseButton>(button));
        event_callback(e);
        break;
      }
    }
  }
}

std::vector<const char*> Window::GetRequiredExtension() {
  std::vector<const char*> extensions;

  uint32_t extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);
  extensions.resize(extension_count);
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data());

  return extensions;
}
