#include "Scene.hpp"

#include <string>
#include <type_traits>

#include "Components/Tag.hpp"
#include "Entity.hpp"

void Scene::CreateEntity(const std::string& tag) {
  Entity entity{this, registry.create()};

  entity.AddComponent<Component::Tag>(tag);
}

void Scene::OnUpdate() {}
