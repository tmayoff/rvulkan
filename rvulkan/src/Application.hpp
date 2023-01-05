#pragma once
#include <vector>
#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include <Renderer.hpp>

#include "VulkanContext.hpp"
#include "Window.hpp"
#include "core/layer.hpp"

class Application {
 public:
  static Application* appInstance;
  static Application* Get() { return appInstance; }

  Application();
  ~Application() = default;
  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  const std::shared_ptr<Window>& GetWindow() { return window; }

  [[nodiscard]] const std::shared_ptr<VulkanContext>& GetVulkanContext() const {
    return vulkan_context;
  }

  void Run();

  void PushLayer(const std::shared_ptr<Layer>& layer);

 private:
  void OnEvent(Event& e);

  bool running = true;

  std::vector<std::shared_ptr<Layer>> layers;

  std::shared_ptr<VulkanContext> vulkan_context;
  std::shared_ptr<Renderer> renderer;

  std::shared_ptr<Window> window;
};

#endif  // APPLICATION_HPP_
