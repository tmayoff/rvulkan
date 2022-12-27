#include "Scene.hpp"

#include <optional>
#include <string>
#include <type_traits>

#include "Components/Camera.hpp"
#include "Components/MeshRenderer.hpp"
#include "Components/Tag.hpp"
#include "Entity.hpp"

Entity Scene::CreateEntity(const std::string& tag) {
  Entity entity{this, registry.create()};
  entity.AddComponent<Component::Tag>(tag);
  return entity;
}

void Scene::OnUpdate() {
  std::optional<Component::Camera> main_camera = std::nullopt;
  auto cameras = registry.view<Component::Camera>();
  for (const auto& cam_entity : cameras) {
    const auto cam = cameras.get<Component::Camera>(cam_entity);

    if (cam.IsPrimary()) {
      main_camera = cam;
      break;
    }
  }

  if (!main_camera) return;

  renderer->StartFrame(main_camera->GetViewMatrix());

  registry.view<Component::MeshRenderer>().each(
      [this](const Component::MeshRenderer& mesh_renderer) { renderer->DrawMesh(mesh_renderer); });

  renderer->EndFrame();
}

void Scene::OnWindowResize(std::pair<float, float> size) { renderer->ResizeViewport(size); }
