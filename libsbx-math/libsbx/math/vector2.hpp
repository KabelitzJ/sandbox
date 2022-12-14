#ifndef LIBSBX_MATH_VECTOR2_HPP_
#define LIBSBX_MATH_VECTOR2_HPP_

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <concepts>
#include <type_traits>

#include <libsbx/math/concepts.hpp>

namespace sbx::math {

/**
 * @brief A vector in two-dimensional space.
 *
 * @tparam Type The type of the vectors components.
 */
template<arithmetic Type>
class basic_vector2 {

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

  /** @brief The origin of two-dimensional space */
  inline static constexpr basic_vector2 zero{value_type{0}, value_type{0}};

  /** @brief A unit vector along the positive x-axis */
  inline static constexpr basic_vector2 right{value_type{-1}, value_type{0}};

  /** @brief A unit vector along the negative x-axis */
  inline static constexpr basic_vector2 left{value_type{1}, value_type{0}};

  /** @brief A unit vector along the positive y-axis */
  inline static constexpr basic_vector2 up{value_type{0}, value_type{-1}};

  /** @brief A unit vector along the negative y-axis */
  inline static constexpr basic_vector2 down{value_type{0}, value_type{1}};

  // -- Data members --

  /** @brief The x-component. */
  value_type x{};
  /** @brief The y-component. */
  value_type y{};

  // -- Constructors --

  /** @brief Constructs a vector with all components set to zero. */
  constexpr basic_vector2() noexcept;

  /**
   * @brief Constructs a vector and assigns all components to the value.
   * 
   * @param value Value for all components.
   */
  explicit constexpr basic_vector2(const value_type value) noexcept;

  /**
   * @brief Constructs a vector and assigns all components to the values.
   * 
   * @param x The value for the x component.
   * @param y The value for the y component.
   */
  constexpr basic_vector2(const value_type x, const value_type y) noexcept;

  /** 
   * @brief Constructs a vector and copies the components from the other vector
   *
   * @param other The other vector to copy the components from. 
   */
  constexpr basic_vector2(const basic_vector2& other) noexcept = default;

  /**
   * @brief Constructs a vector and copies the components from the other vector
   * 
   * @tparam Other The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   */
  template<arithmetic Other>
  explicit constexpr basic_vector2(const basic_vector2<Other>& other) noexcept;

  /** 
   * @brief Constructs a vector and moves the components out of the other vector
   *
   * @param other The other vector to move the components from. 
   */
  constexpr basic_vector2(basic_vector2&& other) noexcept = default;

  /** @brief Destroys the vector */
  ~basic_vector2() noexcept = default;

  // -- Static member functions --

  /**
   * @brief Returns a normalized copy.
   * 
   * @return basic_vector2 The normalized vector.
   */
  [[nodiscard]] static constexpr basic_vector2 normalized(const basic_vector2& vector) noexcept requires (std::floating_point<Type>);

  // -- Assignment operators --

  /**
   * @brief Copies the components from the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector2& A reference to this vector.
   */
  constexpr basic_vector2& operator=(const basic_vector2& other) noexcept = default;

  /**
   * @brief Copies the components from the other vector.
   * 
   * @tparam Other The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector2& A reference to this vector.
   */
  template<arithmetic Other>
  constexpr basic_vector2& operator=(const basic_vector2<Other>& other) noexcept;

  /**
   * @brief Moves the components out of the other vector.
   * 
   * @param other The other vector to move the components from.
   * 
   * @return basic_vector2& A reference to this vector.
   */
  constexpr basic_vector2& operator=(basic_vector2&& other) noexcept = default;

  // -- Unary arithmetic operators --

  /**
   * @brief Negates the vector.
   * 
   * @return basic_vector2& A reference to this vector.
   */
  constexpr basic_vector2& operator-() noexcept;

  // -- Binary arithmetic operators --

  /**
   * @brief Adds the components of the other vector to this vector.
   * 
   * @param other The other vector to add.
   * 
   * @return basic_vector2& A reference to this vector. 
   */
  constexpr basic_vector2& operator+=(const basic_vector2& other) noexcept;

  /**
   * @brief Subtracts the components of the other vector from this vector.
   * 
   * @param other The other vector to subtract.
   * 
   * @return basic_vector2& A reference to this vector. 
   */
  constexpr basic_vector2& operator-=(const basic_vector2& other) noexcept;

  /**
   * @brief Multiplies the components of this vector by the scalar.
   * 
   * @param scalar The scalar to multiply by.
   * 
   * @return basic_vector2& A reference to this vector. 
   */
  constexpr basic_vector2& operator*=(const value_type scalar) noexcept;

  /**
   * @brief Divides the components of this vector by the scalar.
   * 
   * @param scalar The scalar to divide by.
   * 
   * @throws std::domain_error If the scalar is zero.
   * 
   * @return basic_vector2& A reference to this vector. 
   */
  constexpr basic_vector2& operator/=(const value_type scalar);

  // -- Access operators --

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component.
   * 
   * @return reference A reference to the component. 
   */
  [[nodiscard]] constexpr reference operator[](const index_type index) noexcept;

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component. 
   * 
   * @return const_reference A const reference to the component.
   */
  [[nodiscard]] constexpr const_reference operator[](const index_type index) const noexcept;

  // -- Member functions --

  /**
   * @brief Returns the length.
   * 
   * @return value_type The length.
   */
  [[nodiscard]] constexpr length_type length() const noexcept;

  /** @brief Normalizes the vector. */
  constexpr void normalize() noexcept requires (std::floating_point<Type>);

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

}; // class basic_vector2

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
template<arithmetic Type>
[[nodiscard]] constexpr bool operator==(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept;

/**
 * @brief Compares two vectors for inequality.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return true The vectors are not equal.
 * @return false The vectors are equal.
 */
template<arithmetic Type>
[[nodiscard]] constexpr bool operator!=(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept;

// -- Free arithmetic operators --

/**
 * @brief Adds two vectors.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector2<Type> The sum of the two vectors.
 */
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector2<Type> operator+(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept;

/**
 * @brief Subtracts two vectors.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side vector.
 * 
 * @return basic_vector2<Type> The difference of the two vectors. 
 */
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector2<Type> operator-(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept;

/**
 * @brief Multiplies a vector by a scalar. 
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param lhs The left-hand side vector.
 * @param rhs The right-hand side scalar.
 * 
 * @return basic_vector2<Type> The product of the vector and scalar. 
 */
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector2<Type> operator*(basic_vector2<Type> lhs, const Type rhs) noexcept;

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
 * @return basic_vector2<Type> The quotient of the vector and scalar.
 */
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector2<Type> operator/(basic_vector2<Type> lhs, const Type rhs) noexcept;

// -- Type aliases --

/** @brief Type alias for a two-dimensional vector with 32 bit floating-point components. */
using vector2f = basic_vector2<std::float_t>;

/** @brief Type alias for a two-dimensional vector with 32 bit integer components. */
using vector2i = basic_vector2<std::int32_t>;

/** @brief Type alias for vector2f. */
using vector2 = vector2f;

} // namespace sbx::math

#include <libsbx/math/vector2.ipp>

#endif // LIBSBX_MATH_VECTOR2_HPP_
