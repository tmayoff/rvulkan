#ifndef IMGUILAYER_HPP
#define IMGUILAYER_HPP

#include <memory>

#include "layer.hpp"
#include "rvulkan/vulkan_context.hpp"

class Window;
class Renderer;

class ImGuiLayer : public Layer {
 public:
  ImGuiLayer(const std::shared_ptr<Window>& window, std::shared_ptr<VulkanContext>& vulkan_context,
             const std::shared_ptr<Renderer>& renderer);

  void OnAttach() override;
  void OnEvent(Event& /*unused*/) override;

 private:
  static void CreateFontAtlas(std::shared_ptr<VulkanContext>& vulkan_context,
                              const std::shared_ptr<Renderer>& renderer);
};

#endif  // IMGUILAYER_HPP
