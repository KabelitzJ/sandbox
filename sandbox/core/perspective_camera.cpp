#include "perspective_camer.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace sbx {

perspective_camera::perspective_camera(const glm::vec3& position, const glm::vec3& direction, float field_of_view, float aspect_ratio, float near, float far, float pitch, float yaw) 
: camera(position, direction, field_of_view, pitch, yaw), 
  _aspect_ratio(aspect_ratio),
  _near(near),
  _far(far) {

}

glm::mat4 perspective_camera::projection() const {
  return glm::perspective(glm::radians(field_of_view()), _aspect_ratio, _near, _far);
}

} // namespace sbx
