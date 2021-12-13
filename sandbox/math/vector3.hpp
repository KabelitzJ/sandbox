#ifndef SBX_MATH_VECTOR3_HPP_
#define SBX_MATH_VECTOR3_HPP_

#include <types/primitives.hpp>

#include "vector2.hpp"

namespace sbx {

class vector3 {

public:

  vector3();

  vector3(const float32 x, const float32 y, const float32 z);

  vector3(const vector3& other) = default;

  vector3(vector3&& other) = default;

  ~vector3() = default;

  static vector3 cross(const vector3& lhs, const vector3& rhs);

  static float32 dot(const vector3& lhs, const vector3& rhs);

  vector3& operator=(const vector3& other) = default;

  vector3& operator=(vector3&& other) = default;

  constexpr bool operator==(const vector3& other) const;
  constexpr bool operator!=(const vector3& other) const;

  vector3& operator+=(const vector3& other);
  vector3& operator-=(const vector3& other);
  vector3& operator*=(const float32 scalar);
  vector3& operator/=(const float32 scalar);

  vector3 operator-() const;
  vector3& operator-();

  float32 x() const;
  float32 y() const;
  float32 z() const;

  float32& x();
  float32& y();
  float32& z();

  vector2 xy() const;
  vector2 xz() const;
  vector2 yz() const;

  float32 length() const;

  void normalize();
  vector3 normalized() const;

private:

  float32 _x{};
  float32 _y{};
  float32 _z{};

}; // class vector3

vector3 operator+(vector3 lhs, const vector3& rhs);
vector3 operator-(vector3 lhs, const vector3& rhs);
vector3 operator*(vector3 lhs, const float32 rhs);
vector3 operator/(vector3 lhs, const float32 rhs);

} // namespace sbx

#endif // SBX_MATH_VECTOR3_HPP_
