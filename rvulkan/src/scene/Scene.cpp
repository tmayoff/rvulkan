#include "Scene.hpp"

#include <optional>
#include <string>
#include <type_traits>

#include "Components/Camera.hpp"
#include "Components/MeshRenderer.hpp"
#include "Components/Tag.hpp"
#include "Entity.hpp"
#include "entt/entity/fwd.hpp"
#include "renderer/Mesh.hpp"
#include "scene/Components/transform.hpp"

Entity Scene::CreateEntity(const std::string& tag) {
  Entity entity{this, registry.create()};
  entity.AddComponent<Component::Tag>(tag);
  entity.AddComponent<Component::Transform>();
  return entity;
}

void Scene::OnUpdate(const RenderContext& render_context) {
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

  auto camera_matrix = glm::inverse(main_camera->GetTransform().GetObjectToWorld()) *
                       main_camera->GetComponent<Component::Camera>().GetViewMatrix();
  PushConstants push_constant{camera_matrix};
  render_context.PushConstants(&push_constant, sizeof(PushConstants));

  registry.view<Component::MeshRenderer, Component::Transform>().each(
      [render_context](const Component::MeshRenderer& mesh_renderer,
                       const Component::Transform& transform) {
        Renderer::DrawMesh(render_context, mesh_renderer, transform.GetObjectToWorld());
      });
}

void Scene::OnWindowResize(std::pair<float, float> /*size*/) {}
