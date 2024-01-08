#include <libsbx/math/quaternion.hpp>

#include <libsbx/utility/hash.hpp>

namespace sbx::math {

template<scalar Type>
template<scalar Other>
inline constexpr basic_quaternion<Type>::basic_quaternion(Other value) noexcept
: _complex{vector_type{value}},
  _scalar{static_cast<value_type>(value)} { }

template<scalar Type>
template<scalar Other>
inline constexpr basic_quaternion<Type>::basic_quaternion(const vector_type_for<Other>& complex, Other scalar) noexcept
: _complex{complex},
  _scalar{static_cast<value_type>(scalar)} { }

template<scalar Type>
template<scalar Other>
inline constexpr basic_quaternion<Type>::basic_quaternion(Other x, Other y, Other z, Other w) noexcept
: _complex{x, y, z},
  _scalar{static_cast<value_type>(w)} { }

template<scalar Type>
inline constexpr basic_quaternion<Type>::operator matrix_type() const noexcept {
  // [TODO] KAJ 2024-01-06 : Implement this.
  return matrix_type::identity;
}

template<scalar Type>
inline constexpr auto basic_quaternion<Type>::complex() noexcept -> vector_type& {
  return _complex;
}

template<scalar Type>
inline constexpr auto basic_quaternion<Type>::complex() const noexcept -> const vector_type& {
  return _complex;
}

template<scalar Type>
inline constexpr auto basic_quaternion<Type>::scalar() noexcept -> reference {
  return _scalar;
}

template<scalar Type>
inline constexpr auto basic_quaternion<Type>::scalar() const noexcept -> const_reference {
  return _scalar;
}

template<scalar Lhs, scalar Rhs>
inline constexpr auto operator==(const basic_quaternion<Lhs>& lhs, const basic_quaternion<Rhs>& rhs) noexcept -> bool {
  return lhs.complex() == rhs.complex() && lhs.scalar() == rhs.scalar();
}

} // namespace sbx::math

template<sbx::math::scalar Type>
inline auto std::hash<sbx::math::basic_quaternion<Type>>::operator()(const sbx::math::basic_quaternion<Type>& quat) const noexcept -> std::size_t {
  auto seed = std::size_t{0};

  sbx::utility::hash_combine(seed, quat.x);
  sbx::utility::hash_combine(seed, quat.y);
  sbx::utility::hash_combine(seed, quat.z);
  sbx::utility::hash_combine(seed, quat.w);

  return seed;
}
