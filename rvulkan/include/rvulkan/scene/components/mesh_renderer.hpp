#ifndef MESHRENDERER_HPP
#define MESHRENDERER_HPP

#include <rvulkan/renderer/mesh.hpp>
#include <utility>

#include "base.hpp"

namespace Component {

class MeshRenderer : public Base {
 public:
  explicit MeshRenderer(Mesh mesh) : mesh(std::move(mesh)) {}

  [[nodiscard]] const Mesh& GetMesh() const { return mesh; }

 private:
  Mesh mesh;
};

}  // namespace Component
#endif  // MESHRENDERER_HPP
