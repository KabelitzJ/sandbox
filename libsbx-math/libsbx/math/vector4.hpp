#ifndef LIBSBX_MATH_VECTOR4_HPP_
#define LIBSBX_MATH_VECTOR4_HPP_

#include <cstddef>
#include <cmath>
#include <cstdint>
#include <concepts>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <yaml-cpp/yaml.h>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector3.hpp>

namespace sbx::math {

template<numeric Type>
class basic_vector4 {

public:

  // -- Type aliases --

  /** @brief The type of the vector components. */
  using value_type = Type;

  /** @brief The reference type of the vector components. */
  using reference = value_type&;

  /** @brief The const reference type of the vector components. */
  using const_reference = const value_type&;

  /** @brief The pointer type of the vector components. */
  using pointer = value_type*;

  /** @brief The const pointer type of the vector components. */
  using const_pointer = const value_type*;

  /** @brief The type that can describe the length of the vector */
  using length_type = std::float_t;

  /** @brief The type that can index components */
  using index_type = std::size_t; 

  // -- Static data members --

  /** @brief The origin of three dimensional space */
  inline static constexpr basic_vector4 zero{value_type{0}, value_type{0}, value_type{0}, value_type{0}};

  // -- Data members --

  /** @brief The x-component. */
  value_type x{};
  /** @brief The y-component. */
  value_type y{};
  /** @brief The z-component. */
  value_type z{};
  /** @brief The w-component. */
  value_type w{};

  // -- Constructors --

  /** @brief Constructs a vector with all components set to zero. */
  constexpr basic_vector4() noexcept;

  /**
   * @brief Constructs a vector and assigns all components to the value.
   * 
   * @param value Value for all components.
   */
  explicit constexpr basic_vector4(const value_type value) noexcept;

  /**
   * @brief Constructs a vector and assigns all components to the values.
   * 
   * @param x The value for the x component.
   * @param y The value for the y component.
   * @param z The value for the z component.
   * @param w The value for the w component.
   */
  constexpr basic_vector4(const value_type x, const value_type y, const value_type z, const value_type w) noexcept;

  /**
   * @brief Uses a three dimensional vector and a w-component to construct a four dimensional vector.
   * 
   * @param vector Three dimensional vector.
   * @param w The value for the w component. (Default: 0)
   */
  explicit constexpr basic_vector4(const basic_vector3<value_type>& vector, const value_type w = value_type{0}) noexcept;

  /** 
   * @brief Constructs a vector and copies the components from the other vector
   *
   * @param other The other vector to copy the components from. 
   */
  constexpr basic_vector4(const basic_vector4& other) noexcept = default;

  /**
   * @brief Constructs a vector and copies the components from the other vector
   * 
   * @tparam Other The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   */
  template<numeric Other>
  explicit constexpr basic_vector4(const basic_vector4<Other>& other) noexcept;

  /** 
   * @brief Constructs a vector and moves the components out of the other vector
   *
   * @param other The other vector to move the components from. 
   */
  constexpr basic_vector4(basic_vector4&& other) noexcept = default;

  /** @brief Destroys the vector */
  constexpr ~basic_vector4() noexcept = default;

  // -- Static member functions --

  /**
   * @brief Returns a normalized copy.
   * 
   * @return basic_vector4 The normalized vector.
   */
  [[nodiscard]] static constexpr auto normalized(const basic_vector4& vector) noexcept -> basic_vector4;

  // -- Assignment operators --

  /**
   * @brief Copies the components from the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector4& A reference to this vector.
   */
  constexpr auto operator=(const basic_vector4& other) noexcept -> basic_vector4& = default;

  /**
   * @brief Copies the components from the other vector.
   * 
   * @tparam Other The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector4& A reference to this vector.
   */
  template<numeric Other>
  constexpr auto operator=(const basic_vector4<Other>& other) noexcept -> basic_vector4&;

  /**
   * @brief Moves the components out of the other vector.
   * 
   * @param other The other vector to move the components from.
   * 
   * @return basic_vector4& A reference to this vector.
   */
  constexpr auto operator=(basic_vector4&& other) noexcept -> basic_vector4& = default;

  // -- Unary numeric operators --

  /**
   * @brief Negates the vector.
   * 
   * @return basic_vector4& A reference to this vector.
   */
  constexpr auto operator-() noexcept -> basic_vector4&;

  // -- Binary numeric operators --

  /**
   * @brief Adds the components of the other vector to this vector.
   * 
   * @param other The other vector to add.
   * 
   * @return basic_vector4& A reference to this vector. 
   */
  constexpr auto operator+=(const basic_vector4& other) noexcept -> basic_vector4&;

  /**
   * @brief Subtracts the components of the other vector from this vector.
   * 
   * @param other The other vector to subtract.
   * 
   * @return basic_vector4& A reference to this vector. 
   */
  constexpr auto operator-=(const basic_vector4& other) noexcept -> basic_vector4&;

  /**
   * @brief Multiplies the components of this vector by the scalar.
   * 
   * @param scalar The scalar to multiply by.
   * 
   * @return basic_vector4& A reference to this vector. 
   */
  constexpr auto operator*=(const value_type scalar) noexcept -> basic_vector4&;

  /**
   * @brief Multiplies the components of this vector by the vector.
   * 
   * @param other The vector to multiply by.
   * 
   * @return basic_vector4& A reference to this vector.
   */
  constexpr auto operator*=(const basic_vector4& other) noexcept -> basic_vector4&;

  /**
   * @brief Divides the components of this vector by the scalar.
   * 
   * @param scalar The scalar to divide by.
   * 
   * @throws std::domain_error If the scalar is zero.
   * 
   * @return basic_vector4& A reference to this vector. 
   */
  constexpr auto operator/=(const value_type scalar) noexcept -> basic_vector4&;

  /**
   * @brief Divides the components of this vector by the vector.
   * 
   * @param other The vector to divide by.
   * 
   * @return basic_vector4& A reference to this vector. 
   */
  constexpr auto operator/=(const basic_vector4& other) noexcept -> basic_vector4&;

  // -- Access operators --

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component.
   * 
   * @return reference A reference to the component. 
   */
  [[nodiscard]] constexpr auto operator[](const index_type index) noexcept -> reference;

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component. 
   * 
   * @return const_reference A const reference to the component.
   */
  [[nodiscard]] constexpr auto operator[](const index_type index) const noexcept -> const_reference;

  // -- Member functions --

  /**
   * @brief Returns the length.
   * 
   * @return value_type The length.
   */
  [[nodiscard]] constexpr auto length() const noexcept -> length_type;

  /** @brief Normalizes the vector. */
  constexpr void normalize() noexcept;

  // -- Data access --

  /**
   * @brief Return a pointer to the first component.
   *
   * @return pointer A pointer to the first component. 
   */
  [[nodiscard]] constexpr auto data() noexcept -> pointer;

  /**
   * @brief Return a pointer to the first component.
   * 
   * @return const_pointer A pointer to the first component.
   */
  [[nodiscard]] constexpr auto data() const noexcept -> const_pointer;

}; // class basic_vector4

// -- Free comparison operators --

/**
 * @brief Compares two vectors for equality.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return true The vectors are equal.
 * @return false The vectors are not equal.
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator==(const basic_vector4<Type>& lhs, const basic_vector4<Type>& rhs) noexcept -> bool;

// -- Free numeric operators --

/**
 * @brief Adds two vectors.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector4<Type> The sum of the two vectors.
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator+(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept -> basic_vector4<Type>;

/**
 * @brief Subtracts two vectors.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector4<Type> The difference of the two vectors. 
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator-(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept -> basic_vector4<Type>;

/**
 * @brief Multiplies a vector by a scalar. 
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side scalar.
 * 
 * @return basic_vector4<Type> The product of the vector and scalar. 
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator*(basic_vector4<Type> lhs, const Type rhs) noexcept -> basic_vector4<Type>;

/**
 * @brief Multiplies a vector by a vector.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector. 
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector4<Type> The product of the two vectors. 
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator*(basic_vector4<Type> lhs , const basic_vector4<Type>& rhs) noexcept -> basic_vector4<Type>;

/**
 * @brief Divides a vector by a scalar.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side scalar.
 * 
 * @throws std::domain_error If the scalar is zero.
 * 
 * @return basic_vector4<Type> The quotient of the vector and scalar.
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator/(basic_vector4<Type> lhs, const Type rhs) noexcept -> basic_vector4<Type>;

/**
 * @brief Divides a vector by a vector.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector. 
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector4<Type> The quotient of the two vectors. 
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator/(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept -> basic_vector4<Type>;

// -- Type aliases --

/** @brief Type alias for a four-dimensional vector with 32 bit floating-point components. */
using vector4f = basic_vector4<std::float_t>;

/** @brief Type alias for a four-dimensional vector with 32 bit integer components. */
using vector4i = basic_vector4<std::int32_t>;

/** @brief Type alias for vector2f. */
using vector4 = vector4f;

} // namespace sbx::math

template<sbx::math::numeric Type>
struct std::hash<sbx::math::basic_vector4<Type>> {
  auto operator()(const sbx::math::basic_vector4<Type>& vector) const noexcept -> std::size_t;
}; // struct std::hash

template<sbx::math::numeric Type>
struct YAML::convert<sbx::math::basic_vector4<Type>> {
  static auto encode(const sbx::math::basic_vector4<Type>& vector) -> Node;
  static auto decode(const Node& node, sbx::math::basic_vector4<Type>& vector) -> bool;
}; // struct YAML::convert

#include <libsbx/math/vector4.ipp>

#endif // LIBSBX_MATH_VECTOR4_HPP_
