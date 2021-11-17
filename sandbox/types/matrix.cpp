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
  constexpr auto identity = glm::identity<matrix4x4>();

  const auto translation_matrix = glm::translate(identity, transform.position);
  const auto rotation_matrix = to_rotation_matrix(transform.rotation);
  const auto scale_matrix = glm::scale(identity, transform.scale);

  return translation_matrix * rotation_matrix * scale_matrix;
}

matrix4x4 to_rotation_matrix(const quaternion& rotation) {
  return glm::mat4_cast(rotation);
}

} // namespace sbx
