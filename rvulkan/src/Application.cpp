#include "Application.hpp"

#include <events/event.hpp>
#include <events/window_events.hpp>

#include "VulkanContext.hpp"

Application* Application::appInstance = nullptr;

Application::Application() {
  if (appInstance == nullptr) appInstance = this;

  // Create Window
  window = std::make_shared<Window>();
  window->SetEventCallback([this](Event& e) { OnEvent(e); });

  // Initialize Context
  VulkanContextCreateOptions vulkan_options;
  vulkan_options.Layers = {"VK_LAYER_KHRONOS_validation"};
  vulkan_options.Extensions = window->GetRequiredExtension();
  vulkan_context = VulkanContext(vulkan_options, window);
}

void Application::Run() {
  while (running) {
    if (window != nullptr) window->Update();

    for (const auto& l : layers) {
      if (l != nullptr) l->OnUpdate();
    }
  }
}

void Application::PushLayer(const std::shared_ptr<Layer>& layer) {
  layers.push_back(layer);
  layer->OnAttach();
}

void Application::OnEvent(Event& e) {
  Dispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent&) {
    running = false;
    return true;
  });
}
