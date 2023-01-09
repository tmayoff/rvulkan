#ifndef IMGUILAYER_HPP
#define IMGUILAYER_HPP

#include <imgui.h>

#include <memory>

#include "layer.hpp"
#include "rvulkan/vulkan_context.hpp"

class Window;
class Renderer;

class ImGuiLayer : public Layer {
 public:
  ImGuiLayer(const std::shared_ptr<Window>& window, std::shared_ptr<VulkanContext>& vulkan_context,
             const std::shared_ptr<Renderer>& renderer);

  void Begin();
  void End();

  void OnDetach() override;
  void OnUpdate(const std::shared_ptr<RenderContext>& render_context) override;
  void OnEvent(Event& event) override;

 private:
  static void CreateFontAtlas(const std::shared_ptr<Renderer>& renderer);

  ImDrawData* draw_data = nullptr;
};

#endif  // IMGUILAYER_HPP
