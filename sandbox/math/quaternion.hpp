#ifndef SBX_MATH_QUATERNION_HPP_
#define SBX_MATH_QUATERNION_HPP_

#include <cstddef>
#include <concepts>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <meta/concepts.hpp>

#include <types/primitives.hpp>

#include "angle.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "matrix4x4.hpp"

namespace sbx {

/**
 * @brief A quaternion that represents a rotation.
 * 
 * @tparam Type The type of the quaternion components.
 */
template<std::floating_point Type>
struct basic_quaternion {

  // -- Type aliases --

  /** @brief Type of the matrix components */
  using value_type = Type;

  /** @brief The reference type of the matrix components. */
  using reference = value_type&;

  /** @brief The const reference type of the matrix components. */
  using const_reference = const value_type&;

  /** @brief The pointer type of the matrix components. */
  using pointer = value_type*;

  /** @brief The const pointer type of the matrix components. */
  using const_pointer = const value_type*;

  /** @brief The type that can index compotents */
  using index_type = std::size_t;

  // -- Data members --

  /** @brief The x-component of the quaternion. */
  value_type x{};
  /** @brief The y-component of the quaternion. */
  value_type y{};
  /** @brief The z-component of the quaternion. */
  value_type z{};
  /** @brief The w-component of the quaternion. */
  value_type w{};

  // -- Constructors --

  /** @brief Constructs a quaternion with all components set to zero. */
  constexpr basic_quaternion() noexcept;

  constexpr basic_quaternion(const value_type x, const value_type y, const value_type z, const value_type w) noexcept;

  constexpr basic_quaternion(const basic_vector3<value_type>& axis, const angle<value_type>& angle) noexcept;

  constexpr basic_quaternion(const basic_vector3<value_type>& euler_angles) noexcept;

  constexpr basic_quaternion(const basic_quaternion&) noexcept = default;

  template<std::floating_point Other>
  constexpr basic_quaternion(const basic_quaternion<Other>& other) noexcept;

  constexpr basic_quaternion(basic_quaternion&&) noexcept = default;

  /** @brief Destroys the quaternion. */
  ~basic_quaternion() noexcept = default;

  // -- Assignment operators --

  constexpr basic_quaternion& operator=(const basic_quaternion&) noexcept = default;

  template<std::floating_point Other>
  constexpr basic_quaternion& operator=(const basic_quaternion<Other>& other) noexcept;

  constexpr basic_quaternion& operator=(basic_quaternion&&) noexcept = default;

  // -- Access operators --

  /**
   * @brief Returns the component at the given index.
   *
   * @param index The index of the component. 
   * 
   * @return reference The component at the given index.
   */
  [[nodiscard]] constexpr reference operator[](index_type index) noexcept;

  /**
   * @brief Returns the component at the given index.
   *
   * @param index The index of the component. 
   * 
   * @return reference The component at the given index.
   */
  [[nodiscard]] constexpr const_reference operator[](index_type index) const noexcept;

  // -- Data access --

  /**
   * @brief Return a pointer to the first component.
   *
   * @return pointer A pointer to the first component. 
   */
  [[nodiscard]] constexpr pointer data() noexcept;

  /**
   * @brief Return a pointer to the first component.
   * 
   * @return const_pointer A pointer to the first component.
   */
  [[nodiscard]] constexpr const_pointer data() const noexcept;

}; // class quaternion

// -- Free comparison operators --

template<std::floating_point Type>
[[nodiscard]] constexpr bool operator==(const basic_quaternion<Type>& lhs, const basic_quaternion<Type>& rhs) noexcept;

// -- Type aliases --

/** @brief Type alias for a quaternion with 32 bit floating-point components. */
using quaternionf = basic_quaternion<float32>;

/** @brief Type alias for quaternionf */
using quaternion = quaternionf;

} // namespace sbx

#include "quaternion.inl"

#endif // SBX_MATH_QUATERNION_HPP_
