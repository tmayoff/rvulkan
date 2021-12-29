#include "Application.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <memory>
#include <set>
#include <stdexcept>

Application *Application::appInstance = nullptr;

Application::Application() {
  if (appInstance == nullptr) appInstance = this;

  // Create Windwo
  window = std::make_shared<Window>();
  window->SetEventCallback([this](bool quit) { running = !quit; });

  // Initialize Context
  VulkanContextCreateOptions vulkanOptions;
  vulkanOptions.Layers = {"VK_LAYER_KHRONOS_validation"};
  vulkanOptions.Extensions = window->GetRequiredExtension();
  vulkanContext = std::make_shared<VulkanContext>(vulkanOptions);

  vulkanContext->Init(window->GetSurface(vulkanContext->GetInstance()));

  SetCurrentVulkanContext(*vulkanContext.get());

  renderer = std::make_shared<Renderer>();
}

void Application::Run() {
  while (running) {
    window->Update();

    renderer->StartFrame();

    // // Draw things here

    renderer->EndFrame();
  }
}
