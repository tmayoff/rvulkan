#include "Application.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <memory>
#include <set>
#include <stdexcept>

Application *Application::appInstance = nullptr;

Application::Application() {
  if (appInstance == nullptr) appInstance = this;

  window = std::make_shared<Window>();
  window->SetEventCallback([this](bool quit) { running = !quit; });

  createInstance();
  pickPhysicalDevice();
  createSurface();

  renderer = std::make_shared<Renderer>(physicalDevice);
}

void Application::Run() {
  while (running) {
    window->Update();
  }
}

void Application::createInstance() {
  uint32_t extensionCount = 0;
  SDL_Vulkan_GetInstanceExtensions(window->GetWindowHandle(), &extensionCount, nullptr);
  std::vector<const char *> extensions(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(window->GetWindowHandle(), &extensionCount, extensions.data());

  const std::array<const char *, 1> layers = {"VK_LAYER_KHRONOS_validation"};

  vk::ApplicationInfo appInfo("rvulkan", VK_MAKE_VERSION(1, 0, 0), "rvulkan",
                              VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_2);

  vk::InstanceCreateInfo instanceInfo(vk::InstanceCreateFlags(), &appInfo, layers, extensions);
  instance = vk::createInstance(instanceInfo);
}

void Application::createSurface() {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (!SDL_Vulkan_CreateSurface(window->GetWindowHandle(), instance, &surface))
    throw std::runtime_error("Failed to create surface");

  this->surface = surface;
}

void Application::pickPhysicalDevice() {
  auto devices = instance.enumeratePhysicalDevices();

  for (const auto &d : devices) {
    if (UsableDevice(d)) {
      physicalDevice = d;
      break;
    }
  }

  // TODO verify we found a device
  if (!physicalDevice) throw std::runtime_error("Failed to find suitable GPU");
}

bool Application::UsableDevice(vk::PhysicalDevice) {
  // TODO fill this out
  return true;
}
