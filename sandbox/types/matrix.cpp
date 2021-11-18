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
  auto model_matrix = glm::identity<matrix4x4>();

  model_matrix = glm::translate(model_matrix, transform.position);
  model_matrix *= to_rotation_matrix(transform.rotation);
  model_matrix = glm::scale(model_matrix, transform.scale);

  return model_matrix;
}

matrix4x4 to_rotation_matrix(const quaternion& rotation) {
  return glm::mat4_cast(rotation);
}

} // namespace sbx
