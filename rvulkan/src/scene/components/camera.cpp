#include "rvulkan/scene/components/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <variant>

namespace Component {

Camera::Camera(ProjectionType projection_type, float aspect_ratio,
               std::variant<PerspectiveData, OrthographicData> data)
    : aspect_ratio(aspect_ratio),
      projection_type(projection_type),
      projection_data(data),
      view_matrix(1.0F) {
  RecalculateProjection();
}

void Camera::RecalculateProjection() {
  switch (projection_type) {
    case ProjectionType::Orthographic: {
      const auto proj_data = std::get<OrthographicData>(projection_data);
      float left = -proj_data.size * aspect_ratio * 0.5F;
      float right = proj_data.size * aspect_ratio * 0.5F;
      float bottom = -proj_data.size * 0.5F;
      float top = proj_data.size * 0.5F;
      view_matrix = glm::ortho(left, right, bottom, top);

      break;
    }
    case ProjectionType::Projection: {
      const auto proj_data = std::get<PerspectiveData>(projection_data);

      view_matrix = glm::perspective(proj_data.fov, aspect_ratio, proj_data.near, proj_data.far);

      break;
    }
  }
}

}  // namespace Component
