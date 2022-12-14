#ifndef LIBSBX_MATH_VECTOR3_HPP_
#define LIBSBX_MATH_VECTOR3_HPP_

#include <cstddef>
#include <concepts>
#include <type_traits>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector2.hpp>

namespace sbx::math {

/**
 * @brief A vector in three-dimensional space.
 *
 * @tparam Type The type of the vectors components.
 */
template<arithmetic Type>
class basic_vector3 {

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
  template<arithmetic Other>
  explicit constexpr basic_vector3(const basic_vector3<Other>& other) noexcept;

  /** 
   * @brief Constructs a vector and moves the components out of the other vector
   *
   * @param other The other vector to move the components from. 
   */
  constexpr basic_vector3(basic_vector3&& other) noexcept = default;

  /** @brief Destroys the vector */
  ~basic_vector3() noexcept = default;

  // -- Static member functions --

  /**
   * @brief Returns a normalized copy.
   * 
   * @return basic_vector3 The normalized vector.
   */
  [[nodiscard]] static constexpr basic_vector3 normalized(const basic_vector3& vector) noexcept;

  /**
   * @brief Returns the dot product of two vectors.
   * 
   * @param lhs The left hand side vector.
   * @param rhs The right hand side vector.
   * 
   * @return value_type The dot product.
   */
  [[nodiscard]] static constexpr value_type dot(const basic_vector3& lhs, const basic_vector3& rhs) noexcept;

  /**
   * @brief Returns the cross product of two vectors.
   * 
   * @param lhs The left hand side vector.
   * @param rhs The right hand side vector.
   * 
   * @return basic_vector3 The cross product.
   */
  [[nodiscard]] static constexpr basic_vector3 cross(const basic_vector3& lhs, const basic_vector3& rhs) noexcept;

  [[nodiscard]] static constexpr basic_vector3 clamp(const basic_vector3& vector, const value_type min, const value_type max) noexcept;

  // -- Assignment operators --

  /**
   * @brief Copies the components from the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector3& A reference to this vector.
   */
  constexpr basic_vector3& operator=(const basic_vector3& other) noexcept = default;

  /**
   * @brief Copies the components from the other vector.
   * 
   * @tparam Other The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector3& A reference to this vector.
   */
  template<arithmetic Other>
  constexpr basic_vector3& operator=(const basic_vector3<Other>& other) noexcept;

  /**
   * @brief Moves the components out of the other vector.
   * 
   * @param other The other vector to move the components from.
   * 
   * @return basic_vector3& A reference to this vector.
   */
  constexpr basic_vector3& operator=(basic_vector3&& other) noexcept = default;

  // -- Unary arithmetic operators --

  /**
   * @brief Negates the vector.
   * 
   * @return basic_vector3& A reference to this vector.
   */
  constexpr basic_vector3& operator-() noexcept;

  // -- Binary arithmetic operators --

  constexpr basic_vector3& operator+=(const value_type scalar) noexcept;

  /**
   * @brief Adds the components of the other vector to this vector.
   * 
   * @param other The other vector to add.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr basic_vector3& operator+=(const basic_vector3& other) noexcept;

  constexpr basic_vector3& operator-=(const value_type scalar) noexcept;

  /**
   * @brief Subtracts the components of the other vector from this vector.
   * 
   * @param other The other vector to subtract.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr basic_vector3& operator-=(const basic_vector3& other) noexcept;

  /**
   * @brief Multiplies the components of this vector by the scalar.
   * 
   * @param scalar The scalar to multiply by.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr basic_vector3& operator*=(const value_type scalar) noexcept;

  constexpr basic_vector3& operator*=(const basic_vector3& scalar) noexcept;

  template<typename Other>
  requires (std::is_convertible_v<Other, Type>)
  constexpr basic_vector3& operator*=(const Other& scalar) noexcept;

  /**
   * @brief Divides the components of this vector by the scalar.
   * 
   * @param scalar The scalar to divide by.
   * 
   * @throws std::domain_error If the scalar is zero.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr basic_vector3& operator/=(const value_type scalar);

  template<typename Other>
  requires (std::is_convertible_v<Other, Type>)
  constexpr basic_vector3& operator/=(const Other& scalar);

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

  /** 
   * @brief Normalizes the vector.
   */
  constexpr basic_vector3& normalize() noexcept;

  /**
   * @brief Clamps each component between the min and max value.
   * 
   * @param min
   * @param max
   * 
   * @return 
   */
  constexpr basic_vector3& clamp(const value_type min, const value_type max) noexcept;

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
template<arithmetic Type>
[[nodiscard]] constexpr bool operator==(const basic_vector3<Type>& lhs, const basic_vector3<Type>& rhs) noexcept;

// -- Free arithmetic operators --

template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator+(basic_vector3<Type> lhs, const Type rhs) noexcept;

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
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator+(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept;

template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const Type rhs) noexcept;

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
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator-(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept;

/**
 * @brief Creates a negated copy of a vector
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param vector The vector to create the negated copy from
 * 
 * @return basic_vector3<Type> The negated copy of the vector 
 */
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator-(const basic_vector3<Type>& vector) noexcept;

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
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Type rhs) noexcept;

template<arithmetic Type, typename Other>
requires (std::is_convertible_v<Other, Type>)
[[nodiscard]] constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const Other rhs) noexcept;

template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator*(basic_vector3<Type> lhs, const basic_vector3<Type>& rhs) noexcept;

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
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Type rhs);

template<arithmetic Type, typename Other>
requires (std::is_convertible_v<Other, Type>)
[[nodiscard]] constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Other& rhs);

// -- Type aliases --

/** @brief Type alias for a three dimensional vector with 32 bit floating-point components. */
using vector3f = basic_vector3<std::float_t>;

/** @brief Type alias for a three dimensional vector with 32 bit integer components. */
using vector3i = basic_vector3<std::int32_t>;

/** @brief Type alias for vector3f. */
using vector3 = vector3f;

} // namespace sbx::math

#include <libsbx/math/vector3.ipp>

#endif // LIBSBX_MATH_VECTOR3_HPP_
