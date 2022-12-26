#include "Scene.hpp"

#include <string>
#include <type_traits>

#include "Components/MeshRenderer.hpp"
#include "Components/Tag.hpp"
#include "Entity.hpp"

Entity Scene::CreateEntity(const std::string& tag) {
  Entity entity{this, registry.create()};
  entity.AddComponent<Component::Tag>(tag);
  return entity;
}

void Scene::OnUpdate() {
  renderer->StartFrame();

  registry.view<Component::MeshRenderer>().each(
      [this](const Component::MeshRenderer& mesh_renderer) { renderer->DrawMesh(mesh_renderer); });

  renderer->EndFrame();
}
