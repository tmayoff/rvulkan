#include <Application.hpp>
#include <Core/Layer.hpp>
#include <memory>
#include <scene/Entity.hpp>
#include <scene/Scene.hpp>

#include "VulkanContext.hpp"
#include "scene/Components/MeshRenderer.hpp"

class SandboxLayer : public Layer {
 public:
  explicit SandboxLayer(const VulkanContext& vulkan_context) : context(vulkan_context) {
    renderer = std::make_shared<Renderer>(vulkan_context);
  }

  void OnAttach() override;
  void OnUpdate() override;

 private:
  VulkanContext context;

  std::shared_ptr<Scene> scene;
  std::shared_ptr<Renderer> renderer;
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
  auto quad = scene->CreateEntity("Quad");
  quad.AddComponent<Component::MeshRenderer>(Mesh::CreateQuadMesh(context));
}

inline void SandboxLayer::OnUpdate() {
  renderer->StartFrame();

  renderer->DrawQuad();

  scene->OnUpdate();

  renderer->EndFrame();
}
