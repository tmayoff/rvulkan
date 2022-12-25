#include "Application.hpp"

#include "VulkanContext.hpp"

Application *Application::appInstance = nullptr;

Application::Application() {
  if (appInstance == nullptr) appInstance = this;

  // Create Window
  window = std::make_shared<Window>();
  window->SetEventCallback([this](bool quit) { running = !quit; });

  // Initialize Context
  VulkanContextCreateOptions vulkan_options;
  vulkan_options.Layers = {"VK_LAYER_KHRONOS_validation"};
  vulkan_options.Extensions = window->GetRequiredExtension();
  vulkan_context = VulkanContext(vulkan_options, window);

  renderer = std::make_shared<Renderer>(vulkan_context);
}

void Application::Run() {
  while (running) {
    if (window != nullptr) window->Update();

    renderer->StartFrame();

    // Draw things here
    renderer->DrawQuad();

    renderer->EndFrame();
  }
}
