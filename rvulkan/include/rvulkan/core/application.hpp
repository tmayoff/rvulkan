#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>
#include <rvulkan/events/event.hpp>
#include <vector>

#include "layer.hpp"

class VulkanContext;
class Window;
class Renderer;

class Application {
 public:
  Application();

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

#endif  // APPLICATION_HPP
