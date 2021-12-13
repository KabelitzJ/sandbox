#include "vector3.hpp"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

namespace sbx {

inline glm::vec3 to_glm_vector3(const vector3& v) {
  return glm::vec3(v.x(), v.y(), v.z());
}

inline vector3 to_sbx_vector3(const glm::vec3& v) {
  return vector3(v.x, v.y, v.z);
}

vector3::vector3()
: _x{0.0f},
  _y{0.0f},
  _z{0.0f} { }

vector3::vector3(const float32 x, const float32 y, const float32 z)
: _x{x},
  _y{y},
  _z{z} { }

vector3 vector3::cross(const vector3& lhs, const vector3& rhs) {
  const auto cross = glm::cross(to_glm_vector3(lhs), to_glm_vector3(rhs));
  return to_sbx_vector3(cross);
}

float32 dot(const vector3& lhs, const vector3& rhs) {
  return glm::dot(to_glm_vector3(lhs), to_glm_vector3(rhs));
}

constexpr bool vector3::operator==(const vector3& other) const {
  return _x == other._x && _y == other._y && _z == other._z;
}

constexpr bool vector3::operator!=(const vector3& other) const {
  return !(*this == other);
}

vector3& vector3::operator+=(const vector3& other) {
  _x += other._x;
  _y += other._y;
  _z += other._z;

  return *this;
}

vector3& vector3::operator-=(const vector3& other) {
  _x -= other._x;
  _y -= other._y;
  _z -= other._z;

  return *this;
}

vector3& vector3::operator*=(const float32 scalar) {
  _x *= scalar;
  _y *= scalar;
  _z *= scalar;

  return *this;
}

vector3& vector3::operator/=(const float32 scalar) {
  _x /= scalar;
  _y /= scalar;
  _z /= scalar;

  return *this;
}

vector3 vector3::operator-() const {
  return vector3(-_x, -_y, -_z);
}

vector3& vector3::operator-() {
  _x = -_x;
  _y = -_y;
  _z = -_z;

  return *this;
}

float32 vector3::x() const {
  return _x;
}

float32 vector3::y() const {
  return _y;
}

float32 vector3::z() const {
  return _z;
}

float32& vector3::x() {
  return _x;
}

float32& vector3::y() {
  return _y;
}

float32& vector3::z() {
  return _z;
}

vector2 vector3::xy() const {
  return vector2(_x, _y);
}

vector2 vector3::xz() const {
  return vector2(_x, _z);
}

vector2 vector3::yz() const {
  return vector2(_y, _z);
}

float32 vector3::length() const {
  return glm::length(to_glm_vector3(*this));
}

void vector3::normalize() {
  const auto normalized = glm::normalize(to_glm_vector3(*this));
  *this = to_sbx_vector3(normalized);
}

vector3 vector3::normalized() const {
  const auto normalized = glm::normalize(to_glm_vector3(*this));
  return to_sbx_vector3(normalized);
}

vector3 operator+(vector3 lhs, const vector3& rhs) {
  lhs += rhs;
  return lhs;
}

vector3 operator-(vector3 lhs, const vector3& rhs) {
  lhs -= rhs;
  return lhs;
}

vector3 operator*(vector3 lhs, const float32 rhs) {
  lhs *= rhs;
  return lhs;
}

vector3 operator/(vector3 lhs, const float32 rhs) {
  lhs /= rhs;
  return lhs;
}

} // namespace sbx
