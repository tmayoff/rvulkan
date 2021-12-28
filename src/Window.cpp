#include "Window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

Window::Window() {
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("rvulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900,
                            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
}
