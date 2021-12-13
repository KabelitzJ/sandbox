#include "vector2.hpp"

#include <glm/glm.hpp>
#include <glm/vec2.hpp>

namespace sbx {

inline glm::vec2 to_glm_vector2(const vector2& v) {
  return glm::vec2(v.x(), v.y());
}

inline vector2 to_sbx_vector2(const glm::vec2& v) {
  return vector2(v.x, v.y);
}

vector2::vector2()
: _x{0.0f},
  _y{0.0f} { }

vector2::vector2(const float32 x, const float32 y)
: _x{x},
  _y{y} { }

constexpr bool vector2::operator==(const vector2& other) const {
  return _x == other._x && _y == other._y;
}

constexpr bool vector2::operator!=(const vector2& other) const {
  return !(*this == other);
}

vector2& vector2::operator+=(const vector2& other) {
  _x += other._x;
  _y += other._y;

  return *this;
}

vector2& vector2::operator-=(const vector2& other) {
  _x -= other._x;
  _y -= other._y;

  return *this;
}

vector2& vector2::operator*=(const float32 scale) {
  _x *= scale;
  _y *= scale;

  return *this;
}

vector2& vector2::operator/=(const float32 scale) {
  _x /= scale;
  _y /= scale;

  return *this;
}

vector2 vector2::operator-() const {
  return vector2{-_x, -_y};
}

vector2& vector2::operator-() {
  _x = -_x;
  _y = -_y;

  return *this;
}

float32 vector2::x() const {
  return _x;
}

float32 vector2::y() const {
  return _y;
}

float32& vector2::x() {
  return _x;
}

float32& vector2::y() {
  return _y;
}

float32 vector2::length() const {
  return glm::length(to_glm_vector2(*this));
}

void vector2::normalize() {
  const auto normalized = glm::normalize(to_glm_vector2(*this));
  *this = to_sbx_vector2(normalized);
}

vector2 vector2::normalized() const {
  const auto normalized = glm::normalize(to_glm_vector2(*this));
  return to_sbx_vector2(normalized);
}

vector2 operator+(vector2 lhs, const vector2& rhs) {
  lhs += rhs;
  return lhs;
}

vector2 operator-(vector2 lhs, const vector2& rhs) {
  lhs -= rhs;
  return lhs;
}

vector2 operator*(vector2 lhs, const float32 rhs) {
  lhs *= rhs;
  return lhs;
}

vector2 operator/(vector2 lhs, const float32 rhs) {
  lhs /= rhs;
  return lhs;
}

} // namespace sbx
