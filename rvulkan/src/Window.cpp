#include "Window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <events/window_events.hpp>
#include <set>

#include "Application.hpp"
#include "SDL_video.h"

Window::Window()
    : window(SDL_CreateWindow("rvulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)) {
  SDL_Init(SDL_INIT_VIDEO);

  if (window == nullptr) throw std::runtime_error("Failed to create window");
}

void Window::Update() {
  SDL_Event event;

  while (SDL_PollEvent(&event) != 0) {
    switch (event.type) {
      case SDL_QUIT: {
        WindowCloseEvent e;
        event_callback(e);
        break;
      }

      case SDL_WINDOWEVENT_RESIZED: {
        WindowResizeEvent e({event.window.data1, event.window.data2});
        event_callback(e);
        break;
      }
    }
  }
}

std::vector<const char*> Window::GetRequiredExtension() {
  std::vector<const char*> extensions;

  uint32_t extensionCount = 0;
  SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
  extensions.resize(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

  return extensions;
}
