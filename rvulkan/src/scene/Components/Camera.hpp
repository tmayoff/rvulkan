#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <variant>

namespace Component {

enum class ProjectionType { Orthographic, Projection };

struct PerspectiveData {
  float fov = glm::radians(45.0F);
  float near = 0.01F;
  float far = 1000.0F;
};

struct OrthographicData {
  float size = 10.0F;
  float near = -1.0F;
  float far = 1.0F;
};

class Camera {
 public:
  Camera(ProjectionType projection_type, float aspect_ratio,
         std::variant<PerspectiveData, OrthographicData> data);

  [[nodiscard]] bool IsPrimary() const { return is_primary; }

  [[nodiscard]] const glm::mat4& GetViewMatrix() const { return view_matrix; }

 private:
  void RecalculateProjection();

  bool is_primary = true;
  float aspect_ratio;

  ProjectionType projection_type;
  std::variant<PerspectiveData, OrthographicData> projection_data;

  glm::mat4 view_matrix;
};

}  // namespace Component
#endif  // CAMERA_HPP
