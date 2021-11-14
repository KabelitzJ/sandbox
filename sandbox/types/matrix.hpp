#ifndef SBX_TYPES_MATRIX_HPP_
#define SBX_TYPES_MATRIX_HPP_

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "primitives.hpp"
#include "vector.hpp"
#include "quaternion.hpp"
#include "transform.hpp"

namespace sbx {

using matrix3x3 = glm::mat3x3;
using matrix4x4 = glm::mat4x4;

matrix4x4 look_at(const vector3& position, const vector3& target, const vector3& up);

matrix4x4 perspective(float32 fov, float32 aspect, float32 near, float32 far);

matrix4x4 model_matrix_from_transform(const transform& transform);

matrix4x4 to_rotation_matrix(const quaternion& rotation);

} // namespace sbx

#endif // SBX_TYPES_MATRIX_HPP_
