#include <cstdlib>
#include <rvulkan/core/application.hpp>
#include <rvulkan/core/layer.hpp>

#include "imgui.h"

class EditorLayer : public Layer {
 public:
  EditorLayer() : Layer("EditorLayer") {}

  void OnImGuiUpdate() override {
    BeginMainWindow();
    EndMainWindow();
  }

 private:
  static void BeginMainWindow();
  static void EndMainWindow();
};

int main() {
  auto app = Application();
  app.PushLayer(std::make_unique<EditorLayer>());

  app.Run();

  return EXIT_SUCCESS;
}

inline void EditorLayer::BeginMainWindow() {
  // Setup Docking window

  static bool fullscreen = true;
  static bool padding = false;
  static uint32_t dockspace_flags = ImGuiDockNodeFlags_None;

  static uint32_t window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

  if (fullscreen) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  } else {
    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
  }

  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  if (!padding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0F, 0.0F});
  ImGui::Begin("Dockspace Window", nullptr, window_flags);
  if (!padding) ImGui::PopStyleVar();

  if (fullscreen) ImGui::PopStyleVar(2);

  ImGuiIO& io = ImGui::GetIO();
  if ((io.ConfigFlags & ImGuiConfigFlags_DockingEnable) != 0) {
    ImGuiID dockspace_id = ImGui::GetID("Dockspace");
    ImGui::DockSpace(dockspace_id, ImVec2{0.0F, 0.0F}, dockspace_flags);
  }
}

inline void EditorLayer::EndMainWindow() { ImGui::End(); }
