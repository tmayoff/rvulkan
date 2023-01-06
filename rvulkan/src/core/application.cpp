#include "rvulkan/core/application.hpp"

#include <vulkan/vulkan_core.h>

#include <Renderer.hpp>
#include <Window.hpp>
#include <chrono>
#include <debug/profiler.hpp>
#include <rvulkan/core/log.hpp>
#include <rvulkan/events/event.hpp>
#include <rvulkan/events/window_events.hpp>
#include <rvulkan/vulkan_context.hpp>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"
#include "rvulkan/core/imgui_layer.hpp"

Application::Application() {
  logger::Init();

  // Create Window
  window = std::make_shared<Window>();
  window->SetEventCallback([this](Event& e) { OnEvent(e); });

  // Initialize Context
  VulkanContextCreateOptions vulkan_options;
  vulkan_options.Layers = {"VK_LAYER_KHRONOS_validation"};
  vulkan_options.Extensions = window->GetRequiredExtension();
  vulkan_context = std::make_shared<VulkanContext>(vulkan_options, window);

  renderer = std::make_shared<Renderer>(vulkan_context);

  imgui_layer = std::make_shared<ImGuiLayer>(window, vulkan_context, renderer);
  layers.push_back(imgui_layer);
}

void Application::Run() {
  std::chrono::steady_clock::time_point last_loop;
  const auto target_frame_time = std::chrono::milliseconds(16);

  while (running) {
    FrameMark;

    ZoneScopedN("Main Loop");  // NOLINT

    if (window != nullptr) window->Update();

    imgui_layer->Begin();
    for (const auto& l : layers)
      if (l != nullptr) l->OnImGuiUpdate();
    imgui_layer->End();

    renderer->BeginFrame();

    for (const auto& l : layers) {
      if (l != nullptr) l->OnUpdate(renderer->GetRenderContext());
    }

    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_loop);
    if (duration > target_frame_time) {
      auto fps = std::chrono::seconds(1) / duration;
      logger::debug("Frame time: {}ms ({} fps)", duration.count(), fps);
    }
    last_loop = now;

    renderer->EndFrame();
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

  dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
    renderer->ResizeViewport(e.GetSize());
    return false;
  });

  if (e.Handled()) {
    return;
  }

  for (const auto& l : layers) {
    if (!e.Handled()) l->OnEvent(e);
  }
}
