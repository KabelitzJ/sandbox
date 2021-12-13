#ifndef SBX_MATH_VECTOR2_HPP_
#define SBX_MATH_VECTOR2_HPP_

#include <types/primitives.hpp>

namespace sbx {

class vector2 {

public:

  vector2();

  vector2(const float32 x, const float32 y);

  vector2(const vector2& other) = default;

  vector2(vector2&& other) = default;

  ~vector2() = default;

  vector2& operator=(const vector2& other) = default;

  vector2& operator=(vector2&& other) = default;

  constexpr bool operator==(const vector2& other) const;
  constexpr bool operator!=(const vector2& other) const;

  vector2& operator+=(const vector2& other);
  vector2& operator-=(const vector2& other);
  vector2& operator*=(const float32 scale);
  vector2& operator/=(const float32 scale);

  vector2 operator-() const;
  vector2& operator-();

  float32 x() const;
  float32 y() const;

  float32& x();
  float32& y();

  float32 length() const;

  void normalize();
  vector2 normalized() const;

private:

  float32 _x{};
  float32 _y{};

}; // class vector2

vector2 operator+(vector2 lhs, const vector2& rhs);
vector2 operator-(vector2 lhs, const vector2& rhs);
vector2 operator*(vector2 lhs, const float32 rhs);
vector2 operator/(vector2 lhs, const float32 rhs);

} // namespace sbx

#endif // SBX_MATH_VECTOR2_HPP_
