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

float32 length(const vector3& vector) {
  return glm::length(vector);
}

float32 sin(const float32 value) {
  return glm::sin(value);
}

float32 cos(const float32 value) {
  return glm::cos(value);
}

float32 tan(const float32 value) {
  return glm::tan(value);
}

float32 asin(const float32 value) {
  return glm::asin(value);
}

float32 acos(const float32 value) {
  return glm::acos(value);
}

float32 atan(const float32 value) {
  return glm::atan(value);
}

} // namespace sbx
