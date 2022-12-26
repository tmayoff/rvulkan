#ifndef MESHRENDERER_HPP
#define MESHRENDERER_HPP

#include <renderer/Mesh.hpp>
#include <utility>

namespace Component {

class MeshRenderer {
 public:
  explicit MeshRenderer(Mesh mesh) : mesh(std::move(mesh)) {}

  [[nodiscard]] const Mesh& GetMesh() const { return mesh; }

 private:
  Mesh mesh;
};

}  // namespace Component
#endif  // MESHRENDERER_HPP
