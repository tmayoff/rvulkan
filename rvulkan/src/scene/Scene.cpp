#include "Scene.hpp"

#include <optional>
#include <string>
#include <type_traits>

#include "Components/Camera.hpp"
#include "Components/MeshRenderer.hpp"
#include "Components/Tag.hpp"
#include "Entity.hpp"
#include "entt/entity/fwd.hpp"
#include "scene/Components/transform.hpp"

Entity Scene::CreateEntity(const std::string& tag) {
  Entity entity{this, registry.create()};
  entity.AddComponent<Component::Tag>(tag);
  entity.AddComponent<Component::Transform>();
  return entity;
}

void Scene::OnUpdate() {
  std::optional<Entity> main_camera = std::nullopt;
  auto cameras = registry.view<Component::Camera>();
  for (const auto& cam_entity : cameras) {
    const auto cam = cameras.get<Component::Camera>(cam_entity);

    if (cam.IsPrimary()) {
      main_camera = {this, cam_entity};
      break;
    }
  }

  if (!main_camera) return;

  renderer->BeginFrame(glm::inverse(main_camera->GetTransform().GetObjectToWorld()) *
                       main_camera->GetComponent<Component::Camera>().GetViewMatrix());

  registry.view<Component::MeshRenderer, Component::Transform>().each(
      [this](const Component::MeshRenderer& mesh_renderer, const Component::Transform& transform) {
        renderer->DrawMesh(mesh_renderer, transform.GetObjectToWorld());
      });

  renderer->EndFrame();
}

void Scene::OnWindowResize(std::pair<float, float> size) { renderer->ResizeViewport(size); }
