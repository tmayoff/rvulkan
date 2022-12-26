#include <Application.hpp>
#include <Core/Layer.hpp>
#include <memory>
#include <scene/Entity.hpp>
#include <scene/Scene.hpp>
#include <utility>

#include "VulkanContext.hpp"
#include "scene/Components/MeshRenderer.hpp"

class SandboxLayer : public Layer {
 public:
  explicit SandboxLayer(VulkanContext vulkan_context) : context(std::move(vulkan_context)) {}

  void OnAttach() override;
  void OnUpdate() override;

 private:
  VulkanContext context;

  std::shared_ptr<Scene> scene;
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
  auto quad = scene->CreateEntity("Quad");
  quad.AddComponent<Component::MeshRenderer>(Mesh::CreateQuadMesh(context));
}

inline void SandboxLayer::OnUpdate() { scene->OnUpdate(); }
