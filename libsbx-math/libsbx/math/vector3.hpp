#ifndef LIBSBX_MATH_VECTOR3_HPP_
#define LIBSBX_MATH_VECTOR3_HPP_

#include <concepts>
#include <cstddef>
#include <cmath>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <yaml-cpp/yaml.h>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector2.hpp>

namespace sbx::math {

/**
 * @brief A vector in three-dimensional space.
 *
 * @tparam Type The type of the vectors components.
 */
template<numeric Type>
class basic_vector3 {

public:

  // -- Type aliases --

  /** @brief The type of the vector components. */
  using value_type = Type;

  /** @brief The basic_vector3& type of the vector components. */
  using reference = value_type&;

  /** @brief The const basic_vector3& type of the vector components. */
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

  /** @brief A vector with all components set to zero. */
  inline static constexpr basic_vector3 zero{value_type{0}, value_type{0}, value_type{0}};

  /** @brief A unit vector along the positive x-axis */
  inline static constexpr basic_vector3 right{value_type{1}, value_type{0}, value_type{0}};

  /** @brief A unit vector along the negative x-axis */
  inline static constexpr basic_vector3 left{value_type{-1}, value_type{0}, value_type{0}};

  /** @brief A unit vector along the positive y-axis */
  inline static constexpr basic_vector3 up{value_type{0}, value_type{-1}, value_type{0}};

  /** @brief A unit vector along the negative y-axis */
  inline static constexpr basic_vector3 down{value_type{0}, value_type{1}, value_type{0}};

  /** @brief A unit vector along the negative z-axis */
  inline static constexpr basic_vector3 forward{value_type{0}, value_type{0}, value_type{1}};

  /** @brief A unit vector along the positive z-axis */
  inline static constexpr basic_vector3 backward{value_type{0}, value_type{0}, value_type{-1}};

  // -- Data members --

  /** @brief The x-component. */
  value_type x{};
  /** @brief The y-component. */
  value_type y{};
  /** @brief The z-component. */
  value_type z{};

  // -- Constructors --

  /** @brief Constructs a vector with all components set to zero. */
  constexpr basic_vector3() noexcept;

  /**
   * @brief Constructs a vector and assigns all components to the value.
   * 
   * @param value Value for all components.
   */
  explicit constexpr basic_vector3(const value_type value) noexcept;

  /**
   * @brief Constructs a vector and assigns all components to the values.
   * 
   * @param x The value for the x component.
   * @param y The value for the y component.
   * @param z The value for the z component.
   */
  constexpr basic_vector3(const value_type x, const value_type y, const value_type z) noexcept;

  /**
   * @brief Constructs a three dimensional vector from a two dimensional vector.
   * 
   * @param vector A vector to copy the components from.
   * @param z The value for the z component. (Default: 1)
   */
  explicit constexpr basic_vector3(const basic_vector2<value_type>& vector, const value_type z = value_type{0}) noexcept;

  /** 
   * @brief Constructs a vector and copies the components from the other vector
   *
   * @param other The other vector to copy the components from. 
   */
  constexpr basic_vector3(const basic_vector3& other) noexcept = default;

  /**
   * @brief Constructs a vector and copies the components from the other vector
   * 
   * @tparam Other The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   */
  template<numeric Other>
  explicit constexpr basic_vector3(const basic_vector3<Other>& other) noexcept;

  /** 
   * @brief Constructs a vector and moves the components out of the other vector
   *
   * @param other The other vector to move the components from. 
   */
  constexpr basic_vector3(basic_vector3&& other) noexcept = default;

  /** @brief Destroys the vector */
  constexpr ~basic_vector3() noexcept = default;

  // -- Static member functions --

  /**
   * @brief Returns a normalized copy.
   * 
   * @return basic_vector3 The normalized vector.
   */
  [[nodiscard]] static constexpr auto normalized(const basic_vector3& vector) noexcept -> basic_vector3;

  /**
   * @brief Returns the dot product of two vectors.
   * 
   * @param lhs The left hand side vector.
   * @param rhs The right hand side vector.
   * 
   * @return value_type The dot product.
   */
  [[nodiscard]] static constexpr auto dot(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> value_type;

  /**
   * @brief Returns the cross product of two vectors.
   * 
   * @param lhs The left hand side vector.
   * @param rhs The right hand side vector.
   * 
   * @return basic_vector3 The cross product.
   */
  [[nodiscard]] static constexpr auto cross(const basic_vector3& lhs, const basic_vector3& rhs) noexcept -> basic_vector3;

  /**
   * @brief Clamps the components of the vector between the given minimum and maximum values.
   * 
   * @param vector The vector to clamp.
   * @param min The minimum value.
   * @param max The maximum value.
   * 
   * @return basic_vector3 The clamped vector.
   */
  [[nodiscard]] static constexpr auto clamp(const basic_vector3& vector, const value_type min, const value_type max) noexcept -> basic_vector3;

  /**
   * @brief Returns a vector which is the result of linear interpolation between two vectors by a given scale. 
   * 
   * @tparam Scale The type of the scale.
   * 
   * @param lhs The left hand side vector.
   * @param rhs The right hand side vector.
   * @param scale The scale to interpolate by. Must be in the range [0.0, 1.0].
   * 
   * @return basic_vector3 The interpolated vector.
   */
  template<std::floating_point Scale>
  [[nodiscard]] static constexpr auto lerp(const basic_vector3& lhs, const basic_vector3& rhs, const Scale scale) -> basic_vector3;

  // -- Assignment operators --

  /**
   * @brief Copies the components from the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector3& A reference to this vector.
   */
  constexpr auto operator=(const basic_vector3& other) noexcept -> basic_vector3& = default;

  /**
   * @brief Copies the components from the other vector.
   * 
   * @tparam Other The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector3& A reference to this vector.
   */
  template<numeric Other>
  constexpr auto operator=(const basic_vector3<Other>& other) noexcept -> basic_vector3&;

  /**
   * @brief Moves the components out of the other vector.
   * 
   * @param other The other vector to move the components from.
   * 
   * @return basic_vector3& A reference to this vector.
   */
  constexpr auto operator=(basic_vector3&& other) noexcept -> basic_vector3& = default;

  // -- Unary numeric operators --

  /**
   * @brief Negates the vector.
   * 
   * @return basic_vector3& A reference to this vector.
   */
  constexpr auto operator-() noexcept -> basic_vector3&;

  // -- Binary numeric operators --

  constexpr auto operator+=(const value_type scalar) noexcept -> basic_vector3&;

  /**
   * @brief Adds the components of the other vector to this vector.
   * 
   * @param other The other vector to add.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr auto operator+=(const basic_vector3& other) noexcept -> basic_vector3&;

  constexpr auto operator-=(const value_type scalar) noexcept -> basic_vector3&;

  /**
   * @brief Subtracts the components of the other vector from this vector.
   * 
   * @param other The other vector to subtract.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr auto operator-=(const basic_vector3& other) noexcept -> basic_vector3&;

  /**
   * @brief Multiplies the components of this vector by the scalar.
   * 
   * @param scalar The scalar to multiply by.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr auto operator*=(const value_type scalar) noexcept -> basic_vector3&;

  constexpr auto operator*=(const basic_vector3& scalar) noexcept -> basic_vector3&;

  template<numeric Other>
  constexpr auto operator*=(const Other& scalar) noexcept -> basic_vector3&;

  /**
   * @brief Divides the components of this vector by the scalar.
   * 
   * @param scalar The scalar to divide by.
   * 
   * @throws std::domain_error If the scalar is zero.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr auto operator/=(const value_type scalar) -> basic_vector3&;

  constexpr auto operator/=(const basic_vector3& scalar) -> basic_vector3&;

  template<numeric Other>
  constexpr auto operator/=(const Other& scalar) -> basic_vector3&;

  // -- Access operators --

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component.
   * 
   * @return basic_vector3& A reference to the component. 
   */
  [[nodiscard]] constexpr auto operator[](const index_type index) -> reference;

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component. 
   * 
   * @return const basic_vector3& A const basic_vector3& to the component.
   */
  [[nodiscard]] constexpr auto operator[](const index_type index) const -> const_reference;

  // -- Member functions --

  /**
   * @brief Returns the length.
   * 
   * @return value_type The length.
   */
  [[nodiscard]] constexpr auto length() const noexcept -> length_type;

  [[nodiscard]] constexpr auto length_squared() const noexcept -> length_type;

  /** 
   * @brief Normalizes the vector.
   */
  constexpr auto normalize() noexcept -> basic_vector3&;

  /**
   * @brief Clamps each component between the min and max value.
   * 
   * @param min
   * @param max
   * 
   * @return 
   */
  constexpr auto clamp(const value_type min, const value_type max) noexcept -> basic_vector3&;

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

}; // class vector3

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
[[nodiscard]] constexpr auto operator==(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept -> bool;

// -- Free numeric operators --

template<numeric Type>
[[nodiscard]] constexpr auto operator+(basic_vector3<Type> lhs, const Type rhs) noexcept -> basic_vector3<Type>;

/**
 * @brief Adds two vectors.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector3<Type> The sum of the two vectors.
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator+(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept -> basic_vector3<Type>;

template<numeric Type>
[[nodiscard]] constexpr auto operator-(basic_vector3<Type> lhs, const Type rhs) noexcept -> basic_vector3<Type>;

/**
 * @brief Subtracts two vectors.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector3<Type> The difference of the two vectors. 
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator-(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept -> basic_vector3<Type>;

/**
 * @brief Creates a negated copy of a vector
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param vector The vector to create the negated copy from
 * 
 * @return basic_vector3<Type> The negated copy of the vector 
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator-(const basic_vector3<Type>& vector) noexcept -> basic_vector3<Type>;

/**
 * @brief Multiplies a vector by a scalar. 
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side scalar.
 * 
 * @return basic_vector3<Type> The product of the vector and scalar. 
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator*(basic_vector3<Type> lhs, const Type rhs) noexcept -> basic_vector3<Type>;

template<numeric Type>
[[nodiscard]] constexpr auto operator*(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept -> basic_vector3<Type>;

template<numeric Type, numeric Other>
[[nodiscard]] constexpr auto operator*(basic_vector3<Type> lhs, const Other& rhs) noexcept -> basic_vector3<Type>;

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
 * @return basic_vector3<Type> The quotient of the vector and scalar.
 */
template<numeric Type>
[[nodiscard]] constexpr auto operator/(basic_vector3<Type> lhs, const Type rhs) -> basic_vector3<Type>;

template<numeric Type>
[[nodiscard]] constexpr auto operator/(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) -> basic_vector3<Type>;

template<numeric Type, numeric Other>
[[nodiscard]] constexpr auto operator/(basic_vector3<Type> lhs, const Other rhs) -> basic_vector3<Type>;

template<numeric Type>
auto operator<<(std::ostream& output_stream, const basic_vector3<Type>& vector) -> std::ostream&;

// template<numeric Type>
// auto operator<<(io::node& node, const basic_vector3<Type>& vector) -> io::node&;

// -- Type aliases --

/** @brief Type alias for a three dimensional vector with 32 bit floating-point components. */
using vector3f = basic_vector3<std::float_t>;

/** @brief Type alias for a three dimensional vector with 32 bit signed integer components. */
using vector3i = basic_vector3<std::int32_t>;

/** @brief Type alias for a three dimensional vector with 32 bit unsigned integer components. */
using vector3u = basic_vector3<std::uint32_t>;

/** @brief Type alias for vector3f. */
using vector3 = vector3f;

} // namespace sbx::math

/**
 * @brief Specialization of std::hash for sbx::math::basic_vector3.
 * @tparam Type The type of the vectors components.
 */
template<sbx::math::numeric Type>
struct std::hash<sbx::math::basic_vector3<Type>> {
  auto operator()(const sbx::math::basic_vector3<Type>& vector) const noexcept -> std::size_t;
}; // struct std::hash

template<sbx::math::numeric Type>
struct YAML::convert<sbx::math::basic_vector3<Type>> {
  static auto encode(const sbx::math::basic_vector3<Type>& vector) -> Node;
  static auto decode(const Node& node, sbx::math::basic_vector3<Type>& vector) -> bool;
}; // struct YAML::convert

#include <libsbx/math/vector3.ipp>

#endif // LIBSBX_MATH_VECTOR3_HPP_

