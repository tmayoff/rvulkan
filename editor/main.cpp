#include <imgui.h>

#include <cstdlib>
#include <memory>
#include <rvulkan/core/application.hpp>
#include <rvulkan/core/layer.hpp>
#include <rvulkan/scene/components/camera.hpp>
#include <rvulkan/scene/components/mesh_renderer.hpp>
#include <rvulkan/scene/entity.hpp>
#include <rvulkan/scene/scene.hpp>

#include "panels/inspector.hpp"
#include "panels/scene_hierarchy.hpp"

class EditorLayer : public Layer {
 public:
  explicit EditorLayer(std::shared_ptr<VulkanContext> vulkan_context)
      : Layer("EditorLayer"),
        vulkan_context(std::move(vulkan_context)),
        scene(std::make_shared<Scene>()),
        scene_hierarchy(scene),
        inspector(scene) {}

  void OnAttach() override {
    camera = scene->CreateEntity("Main Camera");

    camera.AddComponent<Component::Camera>(Component::ProjectionType::Orthographic, 16.0F / 9.0F,
                                           Component::OrthographicData{});

    auto quad = scene->CreateEntity("Quad");
    quad.AddComponent<Component::MeshRenderer>(Mesh::CreateQuadMesh(vulkan_context));
  }

  void OnImGuiUpdate() override {
    ImGui::ShowDemoWindow();

    BeginMainWindow();

    scene_hierarchy.OnUpdate();
    inspector.SetSelectedEntity(scene_hierarchy.GetSelectedContext());
    inspector.OnUpdate();

    EndMainWindow();
  }

 private:
  static void BeginMainWindow();
  static void EndMainWindow();

  std::shared_ptr<VulkanContext> vulkan_context;

  std::shared_ptr<Scene> scene;

  Entity camera;

  SceneHierarchy scene_hierarchy;
  Inspector inspector;
};

int main() {
  auto app = Application();
  app.PushLayer(std::make_unique<EditorLayer>(app.GetVulkanContext()));

  app.Run();

  return EXIT_SUCCESS;
}

inline void EditorLayer::BeginMainWindow() {
  // Setup Docking window

  static const bool fullscreen = true;
  static const bool padding = false;
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
