#include "Application.hpp"

#include <bits/chrono.h>
#include <vulkan/vulkan_core.h>

#include <Core/Log.hpp>
#include <chrono>
#include <debug/profiler.hpp>
#include <events/event.hpp>
#include <events/window_events.hpp>

#include "VulkanContext.hpp"
#include "tracy/Tracy.hpp"

Application* Application::appInstance = nullptr;

Application::Application() {
  logger::Init();

  if (appInstance == nullptr) appInstance = this;

  // Create Window
  window = std::make_shared<Window>();
  window->SetEventCallback([this](Event& e) { OnEvent(e); });

  // Initialize Context
  VulkanContextCreateOptions vulkan_options;
  vulkan_options.Layers = {"VK_LAYER_KHRONOS_validation"};
  vulkan_options.Extensions = window->GetRequiredExtension();
  vulkan_context = std::make_shared<VulkanContext>(vulkan_options, window);
}

void Application::Run() {
  std::chrono::steady_clock::time_point last_loop;
  const auto target_frame_time = std::chrono::milliseconds(16);

  while (running) {
    FrameMark;

    ZoneScopedN("Main Loop");  // NOLINT

    if (window != nullptr) window->Update();

    for (const auto& l : layers) {
      if (l != nullptr) l->OnUpdate();
    }

    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_loop);
    if (duration > target_frame_time) {
      auto fps = std::chrono::seconds(1) / duration;
      logger::debug("Frame time: {}ms ({} fps)", duration.count(), fps);
    }
    last_loop = now;
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

  if (e.Handled()) {
    return;
  }

  for (const auto& l : layers) {
    if (!e.Handled()) l->OnEvent(e);
  }
}
