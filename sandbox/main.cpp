#include <memory>
#include <rvulkan/core/application.hpp>
#include <rvulkan/core/layer.hpp>
#include <rvulkan/core/log.hpp>
#include <rvulkan/events/window_events.hpp>
#include <rvulkan/renderer/mesh.hpp>
#include <rvulkan/scene/components/camera.hpp>
#include <rvulkan/scene/components/mesh_renderer.hpp>
#include <rvulkan/scene/entity.hpp>
#include <rvulkan/scene/scene.hpp>
#include <rvulkan/vulkan_context.hpp>
#include <utility>

class SandboxLayer : public Layer {
 public:
  explicit SandboxLayer(std::shared_ptr<VulkanContext> vulkan_context)
      : Layer("SandboxLayer"), vulkan_context(std::move(vulkan_context)) {}

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
  app->PushLayer(std::make_unique<SandboxLayer>(app->GetVulkanContext()));
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
