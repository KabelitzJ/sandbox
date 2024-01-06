// #include <cassert>
// #include <cmath>

// #include <libsbx/utility/hash.hpp>

// namespace sbx::math {

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::basic_quaternion() noexcept
// : x{static_cast<Type>(0)}, 
//   y{static_cast<Type>(0)},
//   z{static_cast<Type>(0)}, 
//   w{static_cast<Type>(1)} { }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::basic_quaternion(const Type _x, const Type _y, const Type _z, const Type _w) noexcept
// : x{_x},
//   y{_y},
//   z{_z},
//   w{_w} { }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::basic_quaternion(const basic_vector3<Type>& axis, const basic_angle<Type>& a) noexcept {
//   const auto angle_in_radian = a.to_radians();

//   const auto sin_half_angle = std::sin(angle_in_radian * static_cast<Type>(0.5));
//   const auto cos_half_angle = std::cos(angle_in_radian * static_cast<Type>(0.5));

//   x = axis.x * sin_half_angle;
//   y = axis.y * sin_half_angle;
//   z = axis.z * sin_half_angle;
//   w = cos_half_angle;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::basic_quaternion(const basic_vector3<Type>& euler_angles) noexcept {
//   const auto c = basic_vector3<Type>{
//     std::cos(euler_angles.x * Type{0.5}),
//     std::cos(euler_angles.y * Type{0.5}),
//     std::cos(euler_angles.z * Type{0.5})
//   };

//   const auto s = basic_vector3<Type>{
//     std::sin(euler_angles.x * Type{0.5}),
//     std::sin(euler_angles.y * Type{0.5}),
//     std::sin(euler_angles.z * Type{0.5})
//   };

//   x = s.x * c.y * c.z - c.x * s.y * s.z;
//   y = c.x * s.y * c.z + s.x * c.y * s.z;
//   z = c.x * c.y * s.z - s.x * s.y * c.z;
//   w = c.x * c.y * c.z + s.x * s.y * s.z;
// }

// template<std::floating_point Type>
// template<std::floating_point Other>
// inline constexpr basic_quaternion<Type>::basic_quaternion(const basic_quaternion<Other>& other) noexcept
// : x{static_cast<Type>(other.x)},
//   y{static_cast<Type>(other.y)},
//   z{static_cast<Type>(other.z)},
//   w{static_cast<Type>(other.w)} { }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::value_type basic_quaternion<Type>::dot(const basic_quaternion<Type>& lhs, const basic_quaternion<Type>& rhs) noexcept {
//   return (lhs.x * rhs.x + lhs.y * rhs.y) + (lhs.z * rhs.z + lhs.w * rhs.w);
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> basic_quaternion<Type>::normalized(const basic_quaternion<Type>& quat) noexcept {
//   const auto len = quat.length();

//   if (len <= static_cast<Type>(0)) {
//     return basic_quaternion<Type>{static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(1)};
//   }

//   const auto one_over_len = static_cast<Type>(1) / len;

//   return basic_quaternion<Type>{quat.x * one_over_len, quat.y * one_over_len, quat.z * one_over_len, quat.w * one_over_len};
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> basic_quaternion<Type>::conjugated(const basic_quaternion<Type>& quat) noexcept {
//   return basic_quaternion<Type>{-quat.x, -quat.y, -quat.z, quat.w};
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> basic_quaternion<Type>::inverted(const basic_quaternion<Type>& quat) noexcept {
//   return conjugated(quat) / dot(quat, quat);
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> basic_quaternion<Type>::slerp(const basic_quaternion<Type>& lhs, const basic_quaternion<Type>& rhs, const Type factor) noexcept {
//   auto target = rhs;

//   auto cos_theta = dot(lhs, rhs);

//   // If cosTheta < 0, the interpolation will take the long way around the sphere.
//   // To fix this, one quat must be negated.
//   if(cos_theta < static_cast<Type>(0)) {
//     target = -target;
//     cos_theta = -cos_theta;
//   }

//   // Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
//   if(cos_theta > static_cast<Type>(1) - std::numeric_limits<Type>::epsilon()) {
//     // Linear interpolation
//     return basic_quaternion<Type>{
//       lhs.x * (static_cast<Type>(1) - factor) + target.x * factor,
//       lhs.y * (static_cast<Type>(1) - factor) + target.y * factor,
//       lhs.z * (static_cast<Type>(1) - factor) + target.z * factor,
//       lhs.w * (static_cast<Type>(1) - factor) + target.w * factor,
//     };
//   }

//   // Essential Mathematics, page 467
//   const auto a = std::acos(cos_theta);
//   return (lhs * std::sin((static_cast<Type>(1) - factor) * a) + target * std::sin(factor * a)) / std::sin(a);
// }

// template<std::floating_point Type>
// template<std::floating_point Other>
// inline constexpr basic_quaternion<Type>& basic_quaternion<Type>::operator=(const basic_quaternion<Other>& other) noexcept {
//   x = static_cast<Type>(other.x);
//   y = static_cast<Type>(other.y);
//   z = static_cast<Type>(other.z);
//   w = static_cast<Type>(other.w);

//   return *this;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::operator basic_matrix4x4<Type>() const noexcept {
//   return to_matrix();
// }

// template<std::floating_point Type>
// inline constexpr auto basic_quaternion<Type>::to_matrix() const noexcept -> basic_matrix4x4<Type> {
//   const auto xx = x * x;
//   const auto yy = y * y;
//   const auto zz = z * z;
//   const auto xz = x * z;
//   const auto xy = x * y;
//   const auto yz = y * z;
//   const auto wx = w * x;
//   const auto wy = w * y;
//   const auto wz = w * z;

//   auto result = basic_matrix4x4<Type>::identity;

//   result[0][0] = Type{1} - Type{2} * (yy +  zz);
//   result[0][1] = Type{2} * (xy + wz);
//   result[0][2] = Type{2} * (xz - wy);

//   result[1][0] = Type{2} * (xy - wz);
//   result[1][1] = Type{1} - Type{2} * (xx +  zz);
//   result[1][2] = Type{2} * (yz + wx);

//   result[2][0] = Type{2} * (xz + wy);
//   result[2][1] = Type{2} * (yz - wx);
//   result[2][2] = Type{1} - Type{2} * (xx +  yy);

//   return result;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>& basic_quaternion<Type>::operator+=(const basic_quaternion<Type>& other) noexcept {
//   x += other.x;
//   y += other.y;
//   z += other.z;
//   w += other.w;

//   return *this;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>& basic_quaternion<Type>::operator-=(const basic_quaternion<Type>& other) noexcept {
//   x -= other.x;
//   y -= other.y;
//   z -= other.z;
//   w -= other.w;

//   return *this;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>& basic_quaternion<Type>::operator*=(const Type scalar) noexcept {
//   x *= scalar;
//   y *= scalar;
//   z *= scalar;
//   w *= scalar;

//   return *this;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>& basic_quaternion<Type>::operator*=(const basic_quaternion<Type>& other) noexcept {
//   const auto p = basic_quaternion<Type>{*this};
//   const auto q = basic_quaternion<Type>{other};

//   x =   (p.x * q.w) + (p.y * q.z) - (p.z * q.y) + (p.w * q.x);
//   y =   (p.y * q.w) + (p.z * q.x) - (p.x * q.z) + (p.w * q.y);
//   z =   (p.z * q.w) + (p.x * q.y) - (p.y * q.x) + (p.w * q.z);
//   w = - (p.x * q.x) - (p.y * q.y) - (p.z * q.z) + (p.w * q.w);

//   return *this;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>& basic_quaternion<Type>::operator/=(const Type scalar) {
//   if (scalar == static_cast<Type>(0)) {
//     throw std::domain_error{"Division by zero"};
//   }

//   return *this;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::reference basic_quaternion<Type>::operator[](const index_type index) noexcept {
//   assert(index < 4);

//   switch (index) {
//     default:
//     case 0: {
//       return x;
//     }
//     case 1: {
//       return y;
//     }
//     case 2: {
//       return z;
//     }
//     case 3: {
//       return w;
//     }
//   }
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::const_reference basic_quaternion<Type>::operator[](const index_type index) const noexcept {
//   assert(index < 4);

//   switch (index) {
//     default:
//     case 0: {
//       return x;
//     }
//     case 1: {
//       return y;
//     }
//     case 2: {
//       return z;
//     }
//     case 3: {
//       return w;
//     }
//   }
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::pointer basic_quaternion<Type>::data() noexcept {
//   return &x;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type>::const_pointer basic_quaternion<Type>::data() const noexcept {
//   return &x;
// }

// template<std::floating_point Type>
// constexpr basic_quaternion<Type>::length_type basic_quaternion<Type>::length() const noexcept {
//   return std::sqrt(dot(*this, *this));
// }

// template<std::floating_point Type>
// constexpr basic_quaternion<Type>& basic_quaternion<Type>::normalize() noexcept {
//   const auto len = length();

//   if (len <= static_cast<Type>(0)) {
//     x = static_cast<Type>(0);
//     y = static_cast<Type>(0);
//     z = static_cast<Type>(0);
//     w = static_cast<Type>(1);
//   } else {
//     const auto one_over_len = static_cast<Type>(1) / len;

//     x *= one_over_len;
//     y *= one_over_len;
//     z *= one_over_len;
//     w *= one_over_len;
//   }

//   return *this;
// }

// template<std::floating_point Type>
// constexpr basic_quaternion<Type>& basic_quaternion<Type>::conjugate() noexcept {
//   x *= static_cast<Type>(-1);
//   y *= static_cast<Type>(-1);
//   z *= static_cast<Type>(-1);

//   return *this;
// }

// template<std::floating_point Type>
// inline constexpr bool operator==(const basic_quaternion<Type>& lhs, const basic_quaternion<Type>& rhs) noexcept {
//   return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> operator+(basic_quaternion<Type> quat) noexcept {
//   return quat;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> operator-(basic_quaternion<Type> quat) noexcept {
//   return basic_quaternion<Type>{-quat.x, -quat.y, -quat.z, -quat.w};
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> operator+(basic_quaternion<Type> lhs, const basic_quaternion<Type>& rhs) noexcept {
//   return lhs += rhs;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> operator-(basic_quaternion<Type> lhs, const basic_quaternion<Type>& rhs) noexcept {
//   return lhs -= lhs;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> operator*(basic_quaternion<Type> lhs, const Type rhs) noexcept {
//   return lhs *= rhs;
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> operator*(basic_quaternion<Type> lhs, const basic_quaternion<Type>& rhs) noexcept {
//   return lhs *= rhs;
// }

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_vector3<Type> operator*(basic_quaternion<Type> lhs, const basic_vector3<Type>& rhs) noexcept {
//   const auto v = basic_vector3<Type>{lhs.x, lhs.y, lhs.z};
//   const auto u = basic_vector3<Type>::cross(v, rhs);
//   const auto w = basic_vector3<Type>::cross(v, u);

//   return rhs + ((u * lhs.w) + w) * static_cast<Type>(2);
// }

// template<std::floating_point Type>
// inline constexpr basic_quaternion<Type> operator/(basic_quaternion<Type> lhs, const Type rhs) {
//   return lhs /= rhs;
// }

// } // namespace sbx::math

// template<std::floating_point Type>
// std::size_t std::hash<sbx::math::basic_quaternion<Type>>::operator()(const sbx::math::basic_quaternion<Type>& quat) const noexcept {
//   auto seed = std::size_t{0};
//   sbx::utility::hash_combine(seed, quat.x);
//   sbx::utility::hash_combine(seed, quat.y);
//   sbx::utility::hash_combine(seed, quat.z);
//   sbx::utility::hash_combine(seed, quat.w);
//   return seed;
// }
