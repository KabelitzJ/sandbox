#ifndef SBX_MATH_QUATERNION_HPP_
#define SBX_MATH_QUATERNION_HPP_

#include <array>
#include <cstddef>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

template<typename Type>
struct basic_quaternion {

  // Matrix components must be arithmetic types.
  static_assert(std::is_arithmetic_v<Type>, "Type must be arithmetic");

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

  /** @brief Destorys the quaternion. */
  ~basic_quaternion() noexcept = default;

  // -- Assignment operators --

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

// -- Type aliases --

/** @brief Type alias for a quaternion with 32 bit floating-point components. */
using quaternionf = basic_quaternion<float32>;

/** @brief Type alias for quaterionf */
using quaternion = quaternionf;

} // namespace sbx

#include "quaternion.inl"

#endif // SBX_MATH_QUATERNION_HPP_
