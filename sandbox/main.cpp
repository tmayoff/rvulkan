#include <Application.hpp>
#include <core/layer.hpp>
#include <core/log.hpp>
#include <memory>
#include <scene/Entity.hpp>
#include <scene/Scene.hpp>
#include <utility>

#include "Renderer.hpp"
#include "VulkanContext.hpp"
#include "events/event.hpp"
#include "events/window_events.hpp"
#include "renderer/render_context.hpp"
#include "scene/Components/Camera.hpp"
#include "scene/Components/MeshRenderer.hpp"
#include "scene/Components/transform.hpp"

class SandboxLayer : public Layer {
 public:
  explicit SandboxLayer(std::shared_ptr<VulkanContext> vulkan_context)
      : vulkan_context(std::move(vulkan_context)) {}

  void OnAttach() override;
  void OnUpdate(const RenderContext& render_context) override;
  void OnEvent(Event& e) override;

 private:
  std::shared_ptr<VulkanContext> vulkan_context;
  std::shared_ptr<Scene> scene;

  Entity camera{};
};

int main() {
  auto app = std::make_shared<Application>();

  const auto& context = app->GetVulkanContext();

  auto layer = std::make_shared<SandboxLayer>(context);
  app->PushLayer(layer);

  app->Run();
}

inline void SandboxLayer::OnAttach() {
  scene = std::make_shared<Scene>();

  // Add entities
  camera = scene->CreateEntity("Main Camera");

  camera.AddComponent<Component::Camera>(Component::ProjectionType::Orthographic, 16.0F / 9.0F,
                                         Component::OrthographicData{});

  auto quad = scene->CreateEntity("Quad");
  quad.AddComponent<Component::MeshRenderer>(Mesh::CreateQuadMesh(vulkan_context));
}

inline void SandboxLayer::OnUpdate(const RenderContext& render_context) {
  scene->OnUpdate(render_context);
}

inline void SandboxLayer::OnEvent(Event& e) {
  Dispatcher dispatcher(e);
  dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
    auto [width, height] = e.GetSize();

    auto& cam = this->camera.GetComponent<Component::Camera>();
    cam.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));

    scene->OnWindowResize({width, height});

    return false;
  });
}
