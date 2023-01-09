#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <map>
#include <memory>
#include <rvulkan/events/event.hpp>

#include "imgui_layer.hpp"
#include "layer.hpp"

class VulkanContext;
class Window;
class Renderer;

struct AppSettings {
  bool use_imgui = true;
};

class Application {
 public:
  explicit Application(const AppSettings& app_settings = {});
  ~Application() { running = false; }

  const std::shared_ptr<Window>& GetWindow() { return window; }

  [[nodiscard]] const std::shared_ptr<VulkanContext>& GetVulkanContext() const {
    return vulkan_context;
  }

  void Run();

  void PushLayer(std::unique_ptr<Layer>&& layer);

 private:
  void Close();
  void OnEvent(Event& e);

  bool running = true;

  AppSettings app_settings;

  std::map<std::string, std::unique_ptr<Layer>> layers;

  std::shared_ptr<VulkanContext> vulkan_context;
  std::shared_ptr<Renderer> renderer;

  std::shared_ptr<Window> window;
};

#endif  // APPLICATION_HPP
