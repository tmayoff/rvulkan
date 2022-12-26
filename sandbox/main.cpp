#include <Application.hpp>
#include <Core/Layer.hpp>
#include <memory>
#include <scene/Scene.hpp>

class SandboxLayer : public Layer {
 public:
  void OnAttach() override;
  void OnUpdate() override;

 private:
  std::shared_ptr<Scene> scene;
};

int main() {
  auto app = std::make_shared<Application>();

  auto layer = std::make_shared<SandboxLayer>();
  app->PushLayer(layer);

  app->Run();
}

inline void SandboxLayer::OnAttach() {
  scene = std::make_shared<Scene>();

  // Add entities
}

inline void SandboxLayer::OnUpdate() { scene->OnUpdate(); }
