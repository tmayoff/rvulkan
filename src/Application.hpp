#pragma once
#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include <optional>
#include <vulkan/vulkan.hpp>

#include "Renderer.hpp"
#include "Window.hpp"

class Application {
 public:
  static Application* appInstance;
  static Application* Get() { return appInstance; }

  Application();
  ~Application() = default;
  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  void Run();

  const vk::SurfaceKHR& GetSurface() { return surface; }

 private:
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();

  bool UsableDevice(vk::PhysicalDevice device);

  bool running = true;

  vk::Instance instance;
  vk::SurfaceKHR surface;
  vk::PhysicalDevice physicalDevice;

  std::shared_ptr<Window> window;
  std::shared_ptr<Renderer> renderer;
};

#endif  // APPLICATION_HPP_
