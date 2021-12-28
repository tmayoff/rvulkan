#pragma once
#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <functional>

typedef struct SDL_Window SDL_Window;

class Window {
 public:
  Window();

  void Update();

  void SetEventCallback(std::function<void(bool)> cb) { eventCallback = cb; }

  SDL_Window* GetWindowHandle() const { return window; }

 private:
  SDL_Window* window = nullptr;

  std::function<void(bool)> eventCallback;
};

#endif  // WINDOW_HPP_
