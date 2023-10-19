#include <libsbx/math/matrix4x4.hpp>

#include <algorithm>
#include <iomanip>
#include <cmath>

#include <libsbx/utility/assert.hpp>

namespace sbx::math {

template<numeric Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4() noexcept
: _columns{column_type{0}, column_type{0}, column_type{0}, column_type{0}} { }

template<numeric Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(const std::array<column_type, 4>& columns) noexcept
: _columns{columns} { }

template<numeric Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(
  const column_type& column0,
  const column_type& column1,
  const column_type& column2,
  const column_type& column3
) noexcept
: _columns{column0, column1, column2, column3} { }

template<numeric Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(
  const value_type x0, const value_type x1, const value_type x2, const value_type x3,
  const value_type y0, const value_type y1, const value_type y2, const value_type y3,
  const value_type z0, const value_type z1, const value_type z2, const value_type z3,
  const value_type w0, const value_type w1, const value_type w2, const value_type w3
) noexcept
: _columns{column_type{x0, y0, z0, w0}, column_type{x1, y1, z1, w1}, column_type{x2, y2, z2, w2}, column_type{x3, y3, z3, w3}} { }

template<numeric Type>
template<numeric From>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(const basic_matrix4x4<From>& other) noexcept
: _columns{column_type{other[0]}, column_type{other[1]}, column_type{other[2]}, column_type{other[3]}} { }

template<numeric Type>
inline constexpr auto basic_matrix4x4<Type>::transposed(const basic_matrix4x4& matrix) noexcept -> basic_matrix4x4<Type> {
  auto result = basic_matrix4x4<value_type>{};

  result[0][0] = matrix[0][0];
  result[0][1] = matrix[1][0];
  result[0][2] = matrix[2][0];
  result[0][3] = matrix[3][0];

  result[1][0] = matrix[0][1];
  result[1][1] = matrix[1][1];
  result[1][2] = matrix[2][1];
  result[1][3] = matrix[3][1];

  result[2][0] = matrix[0][2];
  result[2][1] = matrix[1][2];
  result[2][2] = matrix[2][2];
  result[2][3] = matrix[3][2];

  result[3][0] = matrix[0][3];
  result[3][1] = matrix[1][3];
  result[3][2] = matrix[2][3];
  result[3][3] = matrix[3][3];

  return result;
}

template<numeric Type>
inline constexpr auto basic_matrix4x4<Type>::inverted(const basic_matrix4x4& matrix) -> basic_matrix4x4<Type> {
  const auto coef00 = matrix[2][2] * matrix[3][3] - matrix[3][2] * matrix[2][3];
  const auto coef02 = matrix[1][2] * matrix[3][3] - matrix[3][2] * matrix[1][3];
  const auto coef03 = matrix[1][2] * matrix[2][3] - matrix[2][2] * matrix[1][3];

  const auto coef04 = matrix[2][1] * matrix[3][3] - matrix[3][1] * matrix[2][3];
  const auto coef06 = matrix[1][1] * matrix[3][3] - matrix[3][1] * matrix[1][3];
  const auto coef07 = matrix[1][1] * matrix[2][3] - matrix[2][1] * matrix[1][3];

  const auto coef08 = matrix[2][1] * matrix[3][2] - matrix[3][1] * matrix[2][2];
  const auto coef10 = matrix[1][1] * matrix[3][2] - matrix[3][1] * matrix[1][2];
  const auto coef11 = matrix[1][1] * matrix[2][2] - matrix[2][1] * matrix[1][2];

  const auto coef12 = matrix[2][0] * matrix[3][3] - matrix[3][0] * matrix[2][3];
  const auto coef14 = matrix[1][0] * matrix[3][3] - matrix[3][0] * matrix[1][3];
  const auto coef15 = matrix[1][0] * matrix[2][3] - matrix[2][0] * matrix[1][3];

  const auto coef16 = matrix[2][0] * matrix[3][2] - matrix[3][0] * matrix[2][2];
  const auto coef18 = matrix[1][0] * matrix[3][2] - matrix[3][0] * matrix[1][2];
  const auto coef19 = matrix[1][0] * matrix[2][2] - matrix[2][0] * matrix[1][2];

  const auto coef20 = matrix[2][0] * matrix[3][1] - matrix[3][0] * matrix[2][1];
  const auto coef22 = matrix[1][0] * matrix[3][1] - matrix[3][0] * matrix[1][1];
  const auto coef23 = matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1];

  const auto fac0 = basic_vector4<value_type>{coef00, coef00, coef02, coef03};
  const auto fac1 = basic_vector4<value_type>{coef04, coef04, coef06, coef07};
  const auto fac2 = basic_vector4<value_type>{coef08, coef08, coef10, coef11};
  const auto fac3 = basic_vector4<value_type>{coef12, coef12, coef14, coef15};
  const auto fac4 = basic_vector4<value_type>{coef16, coef16, coef18, coef19};
  const auto fac5 = basic_vector4<value_type>{coef20, coef20, coef22, coef23};

  const auto vec0 = basic_vector4<value_type>{matrix[1][0], matrix[0][0], matrix[0][0], matrix[0][0]};
  const auto vec1 = basic_vector4<value_type>{matrix[1][1], matrix[0][1], matrix[0][1], matrix[0][1]};
  const auto vec2 = basic_vector4<value_type>{matrix[1][2], matrix[0][2], matrix[0][2], matrix[0][2]};
  const auto vec3 = basic_vector4<value_type>{matrix[1][3], matrix[0][3], matrix[0][3], matrix[0][3]};

  const auto inv0 = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
  const auto inv1 = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
  const auto inv2 = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
  const auto inv3 = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;

  const auto sign0 = basic_vector4<value_type>{+1, -1, +1, -1};
  const auto sign1 = basic_vector4<value_type>{-1, +1, -1, +1};

  const auto inverse = basic_matrix4x4<value_type>{inv0 * sign0, inv1 * sign1, inv2 * sign0, inv3 * sign1};

  const auto row0 = basic_vector4<value_type>{inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]};

  const auto det0 = matrix[0] * row0;
  
  // [NOTE] KAJ 2022-07-29 00:45 - I dont know why those parentheses are needed here... But im too scared to remove them
  const auto det1 = value_type{(det0.x + det0.y) + (det0.z + det0.w)};

  const auto one_over_determinant = static_cast<value_type>(1) / det1;

  return inverse * one_over_determinant;
}

template<numeric Type>
inline constexpr auto basic_matrix4x4<Type>::look_at(const basic_vector3<value_type>& position, const basic_vector3<value_type>& target, const basic_vector3<value_type>& up) noexcept -> basic_matrix4x4<Type> {
  const auto forward = basic_vector3<value_type>::normalized(target - position);
  const auto right = basic_vector3<value_type>::normalized(basic_vector3<value_type>::cross(forward, up));
  const auto new_up = basic_vector3<value_type>::cross(right, forward);

  auto result = basic_matrix4x4<value_type>::identity;

  result[0][0] = right.x;
  result[1][0] = right.y;
  result[2][0] = right.z;
  result[0][1] = new_up.x;
  result[1][1] = new_up.y;
  result[2][1] = new_up.z;
  result[0][2] = -forward.x;
  result[1][2] = -forward.y;
  result[2][2] = -forward.z;
  result[3][0] = -basic_vector3<value_type>::dot(right, position);
  result[3][1] = -basic_vector3<value_type>::dot(new_up, position);
  result[3][2] = basic_vector3<value_type>::dot(forward, position);

  return result;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> basic_matrix4x4<Type>::perspective(const basic_angle<value_type>& fov, const value_type aspect, const value_type near, const value_type far) noexcept {
  const auto tan_half_fov = std::tan(fov.to_radians() / static_cast<value_type>(2));

  auto result = basic_matrix4x4<value_type>::zero;

  result[0][0] = static_cast<value_type>(1) / (aspect * tan_half_fov);
  result[1][1] = static_cast<value_type>(1) / tan_half_fov;
  result[2][2] = far / (near - far);
  result[2][3] = -static_cast<value_type>(1);
  result[3][2] = -(far * near) / (far - near);

  //// [NOTE] KAJ 2023-10-11 : Flip the y-axis to match Vulkan's coordinate system.
  result[1][1] *= -1;

  return result;
}

template<numeric Type>
inline constexpr auto basic_matrix4x4<Type>::orthographic(const value_type left, const value_type right, const value_type bottom, const value_type top) noexcept -> basic_matrix4x4<Type> {
  auto result = basic_matrix4x4<value_type>::identity;

  result[0][0] = static_cast<value_type>(2) / (right - left);
  result[1][1] = static_cast<value_type>(2) / (top - bottom);
  result[2][2] = -static_cast<value_type>(1);

  result[3][0] = -(right + left) / (right - left);
  result[3][1] = -(top + bottom) / (top - bottom);

  return result;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> basic_matrix4x4<Type>::translated(const basic_matrix4x4<value_type>& matrix, const basic_vector3<value_type>& vector) noexcept {
  auto result = basic_matrix4x4<value_type>{matrix};

  result[3] = matrix[0] * vector[0] + matrix[1] * vector[1] + matrix[2] * vector[2] + matrix[3];

  return result;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> basic_matrix4x4<Type>::rotated(const basic_matrix4x4<value_type>& matrix, const basic_vector3<value_type>& axis, const basic_angle<value_type>& angle) noexcept {
  const auto radians = angle.to_radians();

  const auto cos = std::cos(radians.value());
  const auto sin = std::sin(radians.value());

  const auto normalized_axis = basic_vector3<value_type>::normalized(axis);
  const auto temp = normalized_axis * (static_cast<value_type>(1) - cos); 

  auto rotate = basic_matrix4x4<value_type>{};

  rotate[0][0] = cos + temp[0] * axis[0];
  rotate[0][1] = temp[0] * axis[1] + sin * axis[2];
  rotate[0][2] = temp[0] * axis[2] - sin * axis[1];

  rotate[1][0] = temp[1] * axis[0] - sin * axis[2];
  rotate[1][1] = cos + temp[1] * axis[1];
  rotate[1][2] = temp[1] * axis[2] + sin * axis[0];

  rotate[2][0] = temp[2] * axis[0] + sin * axis[1];
  rotate[2][1] = temp[2] * axis[1] - sin * axis[0];
  rotate[2][2] = cos + temp[2] * axis[2];

  auto result = basic_matrix4x4<value_type>{};

  result[0] = matrix[0] * rotate[0][0] + matrix[1] * rotate[0][1] + matrix[2] * rotate[0][2];
  result[1] = matrix[0] * rotate[1][0] + matrix[1] * rotate[1][1] + matrix[2] * rotate[1][2];
  result[2] = matrix[0] * rotate[2][0] + matrix[1] * rotate[2][1] + matrix[2] * rotate[2][2];
  result[3] = matrix[3];

  return result;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> basic_matrix4x4<Type>::rotation_from_euler_angles(const basic_vector3<value_type>& euler_angles) noexcept {
  const auto cos_roll = std::cos(to_radians(degree{euler_angles.x}).value());
  const auto sin_roll = std::sin(to_radians(degree{euler_angles.x}).value());
  const auto cos_pitch = std::cos(to_radians(degree{euler_angles.y}).value());
  const auto sin_pitch = std::sin(to_radians(degree{euler_angles.y}).value());
  const auto cos_yaw = std::cos(to_radians(degree{euler_angles.z}).value());
  const auto sin_yaw = std::sin(to_radians(degree{euler_angles.z}).value());

  auto yaw = basic_matrix4x4<value_type>::identity;

  yaw[0][0] = cos_yaw;
  yaw[0][1] = sin_yaw;
  yaw[1][0] = -sin_yaw;
  yaw[1][1] = cos_yaw;

  auto pitch = basic_matrix4x4<value_type>::identity;

  pitch[0][0] = cos_pitch;
  pitch[0][2] = -sin_pitch;
  pitch[2][0] = sin_pitch;
  pitch[2][2] = cos_pitch;

  auto roll = basic_matrix4x4<value_type>::identity;

  roll[1][1] = cos_roll;
  roll[1][2] = sin_roll;
  roll[2][1] = -sin_roll;
  roll[2][2] = cos_roll;

  return yaw * pitch * roll;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> basic_matrix4x4<Type>::scaled(const basic_matrix4x4<value_type>& matrix, const basic_vector3<value_type>& vector) noexcept {
  auto result = basic_matrix4x4<value_type>{};

  result[0] = matrix[0] * vector[0];
  result[1] = matrix[1] * vector[1];
  result[2] = matrix[2] * vector[2];
  result[3] = matrix[3];

  return result;
}

template<numeric Type>
template<numeric From>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator=(const basic_matrix4x4<From>& other) noexcept {
  _columns[0] = column_type{other[0]};
  _columns[1] = column_type{other[1]};
  _columns[2] = column_type{other[2]};
  _columns[3] = column_type{other[3]};

  return *this;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator+=(const basic_matrix4x4<Type>& other) noexcept {
  _columns[0] += other[0];
  _columns[1] += other[1];
  _columns[2] += other[2];
  _columns[3] += other[3];

  return *this;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator-=(const basic_matrix4x4<Type>& other) noexcept {
  _columns[0] -= other[0];
  _columns[1] -= other[1];
  _columns[2] -= other[2];
  _columns[3] -= other[3];

  return *this;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator*=(const Type scalar) noexcept {
  _columns[0] *= scalar;
  _columns[1] *= scalar;
  _columns[2] *= scalar;
  _columns[3] *= scalar;

  return *this;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type>::column_type_reference basic_matrix4x4<Type>::operator[](const index_type index) {
  if (index >= 4) {
    throw std::out_of_range{"Invalid index for 4x4 matrix"};
  }

  return _columns[index];
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type>::const_column_type_reference basic_matrix4x4<Type>::operator[](const index_type index) const {
  if (index >= 4) {
    throw std::out_of_range{"Invalid index for 4x4 matrix"};
  }

  return _columns[index];
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type>::pointer basic_matrix4x4<Type>::data() noexcept {
  return &_columns[0].x;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type>::const_pointer basic_matrix4x4<Type>::data() const noexcept {
  return &_columns[0].x;
}

template<numeric Type>
constexpr void basic_matrix4x4<Type>::transpose() noexcept {
  *this = transposed(*this);
}

template<numeric Type>
constexpr void basic_matrix4x4<Type>::inverse() noexcept {
  *this = inverted(*this);
}

template<numeric Type>
inline constexpr bool operator==(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> operator+(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> operator-(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> operator*(basic_matrix4x4<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<numeric Type>
inline constexpr basic_vector4<Type> operator*(basic_matrix4x4<Type> lhs, const basic_vector4<Type>& rhs) noexcept {

  // [NOTE] KAJ 2022-02-04 23:42 - This might become a performance bottleneck in the future. But most matrix multiplications are going to happen on the GPU anyways.
  // const auto mov0 = rhs[0];
  // const auto mov1 = rhs[1];

  // const auto mul0 = lhs[0] * mov0;
  // const auto mul1 = lhs[1] * mov1;

  // const auto add0 = mul0 + mul1;

  // const auto mov2 = rhs[2];
  // const auto mov3 = rhs[3];

  // const auto mul2 = lhs[2] * mov2;
  // const auto mul3 = lhs[3] * mov3;

  // const auto add1 = mul2 + mul3;

  // return add0 + add1;
  
  return basic_vector4<Type>{
    lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2] + lhs[0][3] * rhs[3],
    lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2] + lhs[1][3] * rhs[3],
    lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2] + lhs[2][3] * rhs[3],
    lhs[3][0] * rhs[0] + lhs[3][1] * rhs[1] + lhs[3][2] * rhs[2] + lhs[3][3] * rhs[3]
  };
}

template<numeric Type>
inline constexpr basic_matrix4x4<Type> operator*(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  const auto lhs0 = lhs[0];
  const auto lhs1 = lhs[1];
  const auto lhs2 = lhs[2];
  const auto lhs3 = lhs[3];

  const auto rhs0 = rhs[0];
  const auto rhs1 = rhs[1];
  const auto rhs2 = rhs[2];
  const auto rhs3 = rhs[3];

  // [NOTE] KAJ 2022-02-04 23:42 - This might become a performance bottleneck in the future. But most matrix multiplications are going to happen on the GPU anyways.
  auto result = basic_matrix4x4<Type>{};

  result[0] = lhs0 * rhs0[0] + lhs1 * rhs0[1] + lhs2 * rhs0[2] + lhs3 * rhs0[3];
  result[1] = lhs0 * rhs1[0] + lhs1 * rhs1[1] + lhs2 * rhs1[2] + lhs3 * rhs1[3];
  result[2] = lhs0 * rhs2[0] + lhs1 * rhs2[1] + lhs2 * rhs2[2] + lhs3 * rhs2[3];
  result[3] = lhs0 * rhs3[0] + lhs1 * rhs3[1] + lhs2 * rhs3[2] + lhs3 * rhs3[3];

  return result;
}

} // namespace sbx::math
