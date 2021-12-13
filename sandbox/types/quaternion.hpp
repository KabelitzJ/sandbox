#ifndef SBX_TYPES_QUATERNION_HPP_
#define SBX_TYPES_QUATERNION_HPP_

#include <glm/gtc/quaternion.hpp>

#include "primitives.hpp"
#include "vector.hpp"

namespace sbx {

using quaternion = glm::quat;

quaternion rotate(const quaternion& q, const vector3& v);

quaternion from_points(const vector3& start, const vector3& end);

} // namespace sbx

#endif // SBX_TYPES_QUATERNION_HPP_
