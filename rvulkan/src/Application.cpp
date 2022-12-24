#include "Application.hpp"

#include "VulkanContext.hpp"

Application *Application::appInstance = nullptr;

Application::Application() {
  if (appInstance == nullptr) appInstance = this;

  // Create Window
  window = std::make_shared<Window>();
  window->SetEventCallback([this](bool quit) { running = !quit; });

  // Initialize Context
  VulkanContextCreateOptions vulkanOptions;
  vulkanOptions.Layers = {"VK_LAYER_KHRONOS_validation"};
  vulkanOptions.Extensions = window->GetRequiredExtension();
  vulkan_context = VulkanContext(vulkanOptions);

  vulkan_context.Init(window->GetSurface(vulkan_context.GetInstance()));

  SetCurrentVulkanContext(vulkan_context);

  renderer = std::make_shared<Renderer>(vulkan_context);
}

void Application::Run() {
  while (running) {
    window->Update();

    // renderer->StartFrame();

    // // Draw things here
    // renderer->DrawQuad();

    // renderer->EndFrame();
  }
}
