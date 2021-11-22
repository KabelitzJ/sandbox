#include "vector.hpp"

namespace sbx {

float32 to_radians(float32 degrees) {
  return glm::radians(degrees);
}

float32 to_degrees(float32 radians) {
  return glm::degrees(radians);
}

vector3 cross(const vector3& lhs, const vector3& rhs) {
  return glm::cross(lhs, rhs);
}

vector3 normalize(const vector3& vector) {
  return glm::normalize(vector);
}

} // namespace sbx
