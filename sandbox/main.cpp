#include <Application.hpp>
#include <Core/Layer.hpp>
#include <Core/Log.hpp>
#include <memory>
#include <scene/Entity.hpp>
#include <scene/Scene.hpp>
#include <utility>

#include "VulkanContext.hpp"
#include "events/event.hpp"
#include "events/window_events.hpp"
#include "scene/Components/Camera.hpp"
#include "scene/Components/MeshRenderer.hpp"

class SandboxLayer : public Layer {
 public:
  explicit SandboxLayer(VulkanContext vulkan_context) : context(std::move(vulkan_context)) {}

  void OnAttach() override;
  void OnUpdate() override;
  void OnEvent(Event& e) override;

 private:
  VulkanContext context;

  std::shared_ptr<Scene> scene;

  Entity camera;
};

int main() {
  auto app = std::make_shared<Application>();

  const auto& context = app->GetVulkanContext();

  auto layer = std::make_shared<SandboxLayer>(context);
  app->PushLayer(layer);

  app->Run();
}

inline void SandboxLayer::OnAttach() {
  scene = std::make_shared<Scene>(context);

  // Add entities
  camera = scene->CreateEntity("Main Camera");

  camera.AddComponent<Component::Camera>(Component::ProjectionType::Orthographic, 16.0F / 9.0F,
                                         Component::OrthographicData{});

  auto quad = scene->CreateEntity("Quad");
  quad.AddComponent<Component::MeshRenderer>(Mesh::CreateQuadMesh(context));
}

inline void SandboxLayer::OnUpdate() { scene->OnUpdate(); }

inline void SandboxLayer::OnEvent(Event& e) {
  Dispatcher dispatcher(e);
  dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
    logger::info("Window resized");

    auto [width, height] = e.GetSize();

    auto& cam = this->camera.GetComponent<Component::Camera>();
    cam.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));

    return false;
  });
}
