#include "quaternion.hpp"

#include <glm/gtx/quaternion.hpp>

namespace sbx {

quaternion rotate(const quaternion& q, const vector3& v) {
  return glm::rotate(q, v);
}

quaternion from_points(const vector3& start, const vector3& end) {
  const auto cross_product = cross(start, end);
  const auto sine_of_angle = length(cross_product);

  const auto angle = asin(sine_of_angle);
  const auto axis = cross_product / sine_of_angle;

  const auto imaginary = sin(angle / 2.0f) * axis;

  
  return quaternion{imaginary.x, imaginary.y, imaginary.z, cos(angle / 2.0f)};
}

} // namespace sbx
