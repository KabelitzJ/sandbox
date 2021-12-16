#ifndef SBX_MATH_VECTOR4_HPP_
#define SBX_MATH_VECTOR4_HPP_

#include <types/primitives.hpp>

namespace sbx {

class vector4 {

public:

  vector4();

  vector4(const float32 x, const float32 y, const float32 z, const float32 w);

  vector4(const vector4& other) = default;

  vector4(vector4&& other) = default;

  ~vector4() = default;

  vector4& operator=(const vector4& other) = default;

  vector4& operator=(vector4&& other) = default;

  constexpr bool operator==(const vector4& other) const;
  constexpr bool operator!=(const vector4& other) const;

  vector4& operator+=(const vector4& other);
  vector4& operator-=(const vector4& other);
  vector4& operator*=(const float32 scalar);
  vector4& operator/=(const float32 scalar);

  vector4 operator-() const;
  vector4& operator-();

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
  vector4 normalized() const;

private:

  float32 _x{};
  float32 _y{};
  float32 _z{};
  float32 _w{};

}; // class vector4

} // namespace sbx

#endif // SBX_MATH_VECTOR4_HPP_
