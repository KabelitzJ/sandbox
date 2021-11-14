#include "matrix.hpp"

#include <glm/gtx/transform.hpp>

namespace sbx {

matrix4x4 look_at(const vector3& position, const vector3& target, const vector3& up) {
  return glm::lookAt(position, target, up);
}

matrix4x4 perspective(float32 fov, float32 aspect, float32 near, float32 far) {
  return glm::perspective(fov, aspect, near, far);
}

matrix4x4 model_matrix_from_transform(const transform& transform) {
  auto matrix = matrix4x4{};

  matrix = glm::translate(matrix, transform.position);
  matrix *= to_rotation_matrix(transform.rotation);
  matrix = glm::scale(matrix, transform.scale);

  return matrix;
}

matrix4x4 to_rotation_matrix(const quaternion& rotation) {
  return glm::mat4_cast(rotation);
}

} // namespace sbx
