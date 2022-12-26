#pragma once
#include <vector>
#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include <Renderer.hpp>

#include "Core/Layer.hpp"
#include "VulkanContext.hpp"
#include "Window.hpp"

class Application {
 public:
  static Application* appInstance;
  static Application* Get() { return appInstance; }

  Application();
  ~Application() = default;
  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  const std::shared_ptr<Window>& GetWindow() { return window; }

  [[nodiscard]] const VulkanContext& GetVulkanContext() const { return vulkan_context; }

  void Run();

  void PushLayer(const std::shared_ptr<Layer>& layer);

 private:
  bool running = true;

  std::vector<std::shared_ptr<Layer>> layers;

  VulkanContext vulkan_context;
  std::shared_ptr<Window> window;
  std::shared_ptr<Renderer> renderer;
};

#endif  // APPLICATION_HPP_
