#include <cstdlib>
#include <rvulkan/core/application.hpp>
#include <rvulkan/core/layer.hpp>

class EditorLayer : public Layer {
 public:
  EditorLayer() : Layer("EditorLayer") {}

  void OnImGuiUpdate() override { ImGui::ShowDemoWindow(); }
};

int main() {
  auto app = Application();
  app.PushLayer(std::make_shared<EditorLayer>());

  app.Run();

  return EXIT_SUCCESS;
}
