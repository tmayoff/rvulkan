#include "rvulkan/core/imgui_layer.hpp"

#include <vulkan/vulkan_core.h>

#include <Renderer.hpp>
#include <Window.hpp>
#include <rvulkan/core/log.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"
#include "rvulkan/events/mouse_codes.hpp"
#include "rvulkan/events/mouse_events.hpp"

ImGuiLayer::ImGuiLayer(const std::shared_ptr<Window>& window,
                       std::shared_ptr<VulkanContext>& vulkan_context,
                       const std::shared_ptr<Renderer>& renderer)
    : Layer("ImGuiLayer") {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplSDL2_InitForVulkan(window->GetWindowHandle());
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vulkan_context->GetInstance();
  init_info.PhysicalDevice = vulkan_context->GetPhysicalDevice()->GetHandle();
  init_info.Device = vulkan_context->GetLogicalDevice()->GetHandle();
  init_info.QueueFamily = vulkan_context->GetLogicalDevice()->GetIndices().graphics_family.value();
  init_info.Queue = vulkan_context->GetLogicalDevice()->GetGraphicsQueue();
  init_info.DescriptorPool = vulkan_context->GetDescriptorPool();
  init_info.MinImageCount = 2;
  init_info.ImageCount = renderer->GetRenderContext().GetSwapchain().GetImageViews().size();
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

  bool inited =
      ImGui_ImplVulkan_Init(&init_info, renderer->GetRenderContext().GetRenderPass()->GetHandle());

  if (!inited) logger::fatal("Failed to init imgui vulkan");

  CreateFontAtlas(vulkan_context, renderer);
}

void ImGuiLayer::Begin() {
  // Do ImGui First to get the drawn elements
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame();

  ImGui::NewFrame();
}

void ImGuiLayer::End() {
  ImGui::Render();
  draw_data = ImGui::GetDrawData();
}

void ImGuiLayer::OnUpdate(const RenderContext& render_context) {
  ImGui_ImplVulkan_RenderDrawData(draw_data, render_context.GetCurrentCommandBuffer());
}

void ImGuiLayer::OnEvent(Event& event) {
  ImGuiIO& io = ImGui::GetIO();

  Dispatcher dispatcher(event);
  dispatcher.Dispatch<MouseButtonPressedEvent>([&io](MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      io.MouseDown[0] = true;
    } else if (e.GetMouseButton() == MouseButton::ButtonMiddle) {
      io.MouseDown[2] = true;
    } else if (e.GetMouseButton() == MouseButton::ButtonRight) {
      io.MouseDown[1] = true;
    }

    return false;
  });

  dispatcher.Dispatch<MouseButtonReleasedEvent>([&io](MouseButtonReleasedEvent& e) {
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      io.MouseDown[0] = false;
    } else if (e.GetMouseButton() == MouseButton::ButtonMiddle) {
      io.MouseDown[2] = false;
    } else if (e.GetMouseButton() == MouseButton::ButtonRight) {
      io.MouseDown[1] = false;
    }

    return false;
  });
}

void ImGuiLayer::CreateFontAtlas(std::shared_ptr<VulkanContext>& vulkan_context,
                                 const std::shared_ptr<Renderer>& renderer) {
  // TODO Clean this up, manually setting this all is a bit weird

  auto cmd_buffer = renderer->GetRenderContext().GetCurrentCommandBuffer();

  vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  cmd_buffer.begin(begin_info);

  ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
  cmd_buffer.end();

  vk::SubmitInfo submit_info({}, {}, cmd_buffer);

  vulkan_context->GetLogicalDevice()->GetGraphicsQueue().submit(submit_info);
  vulkan_context->GetLogicalDevice()->GetHandle().waitIdle();

  ImGui_ImplVulkan_DestroyFontUploadObjects();
}
