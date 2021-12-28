#pragma once
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

typedef struct SDL_Window SDL_Window;

class Window {
 public:
  Window();

  SDL_Window* GetWindowHandle() const { return window; }

 private:
  SDL_Window* window = nullptr;
};

#endif  // WINDOW_HPP_
