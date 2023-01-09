#include "rvulkan/core/application.hpp"

#include <vulkan/vulkan_core.h>

#include <Renderer.hpp>
#include <Window.hpp>
#include <chrono>
#include <debug/profiler.hpp>
#include <memory>
#include <rvulkan/core/log.hpp>
#include <rvulkan/events/event.hpp>
#include <rvulkan/events/window_events.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <vulkan/LogicalDevice.hpp>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"
#include "rvulkan/core/imgui_layer.hpp"

Application::Application(const AppSettings& app_settings) : app_settings(app_settings) {
  logger::Init(spdlog::level::debug);

  // Create Window
  window = std::make_shared<Window>();
  window->SetEventCallback([this](Event& e) { OnEvent(e); });

  // Initialize Context
  VulkanContextCreateOptions vulkan_options;
  vulkan_options.Layers = {"VK_LAYER_KHRONOS_validation"};
  vulkan_options.Extensions = window->GetRequiredExtension();
  vulkan_context = std::make_shared<VulkanContext>(vulkan_options, window);

  renderer = std::make_shared<Renderer>(vulkan_context);

  if (app_settings.use_imgui)
    PushLayer(std::make_unique<ImGuiLayer>(window, vulkan_context, renderer));
}

void Application::Run() {
  std::chrono::steady_clock::time_point last_loop;
  const auto target_frame_time = std::chrono::milliseconds(8);

  while (running) {
    FrameMark;

    ZoneScopedN("Main Loop");  // NOLINT

    if (window != nullptr) window->Update();

    if (app_settings.use_imgui) {
      reinterpret_cast<ImGuiLayer*>(layers.at("ImGuiLayer").get())->Begin();
      for (const auto& l : layers)
        if (l.second != nullptr) l.second->OnImGuiUpdate();
      reinterpret_cast<ImGuiLayer*>(layers.at("ImGuiLayer").get())->End();
    }

    renderer->BeginFrame();

    for (const auto& l : layers) {
      if (l.second != nullptr) l.second->OnUpdate(renderer->GetRenderContext());
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

  Close();
}

void Application::PushLayer(std::unique_ptr<Layer>&& layer) {
  layer->OnAttach();
  auto name = layer->GetLayerName();
  layers.emplace(name, std::move(layer));
}

void Application::Close() {
  ZoneScoped;  // NOLINT

  vulkan_context->GetLogicalDevice()->GetHandle().waitIdle();

  // Remove layers
  auto it = layers.begin();
  for (; it != layers.end();) {
    (*it).second->OnDetach();
    it = layers.erase(it);
  }

  renderer.reset();
  vulkan_context.reset();
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
    if (!e.Handled()) l.second->OnEvent(e);
  }
}
