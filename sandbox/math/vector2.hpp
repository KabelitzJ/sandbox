#ifndef SBX_MATH_VECTOR2_HPP_
#define SBX_MATH_VECTOR2_HPP_

#include <ostream>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

/**
 * @brief A vector in two-dimensional space.
 *
 * @tparam Type The type of the vectors components.
 */
template<typename Type>
struct basic_vector2 {

  // Vector components can only be arithmetic types.
  static_assert(std::is_arithmetic_v<Type>, "Type must be arithmetic");

  // -- Type aliases --

  /** @brief The type of the vector components. */
  using value_type = Type;

  /** @brief The that can describe the length of the vector */
  using length_type = float32;

  // -- Static data members --

  /** @brief The origin of two-dimensional space */
  inline static constexpr basic_vector2<value_type> origin{value_type{0}, value_type{0}};

  /** @brief A unit vector along the positive x-axis */
  inline static constexpr basic_vector2<value_type> right{value_type{1}, value_type{0}};

  /** @brief A unit vector along the negative x-axis */
  inline static constexpr basic_vector2<value_type> left{value_type{-1}, value_type{0}};

  /** @brief A unit vector along the positive y-axis */
  inline static constexpr basic_vector2<value_type> up{value_type{0}, value_type{1}};

  /** @brief A unit vector along the negative y-axis */
  inline static constexpr basic_vector2<value_type> down{value_type{0}, value_type{-1}};

  // -- Data members --

  /** @brief The x-component of the vector. */
  value_type x{};
  /** @brief The y-component of the vector. */
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
  constexpr basic_vector2(const basic_vector2<value_type>&) noexcept = default;

  /**
   * @brief Constructs a vector and copies the components from the other vector
   * 
   * @tparam From The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   */
  template<typename From>
  constexpr basic_vector2(const basic_vector2<From>& other) noexcept;

  /** 
   * @brief Constructs a vector and moves the components out of the other vector
   *
   * @param other The other vector to move the components from. 
   */
  constexpr basic_vector2(basic_vector2<value_type>&&) noexcept = default;

  /** @brief Destroys the vector */
  ~basic_vector2() noexcept = default;

  // -- Static member functions --

  /**
   * @brief Returns a normalized copy of the vector.
   * 
   * @return basic_vector2<value_type> The normalized vector.
   */
  [[nodiscard]] static constexpr basic_vector2<value_type> normalized(const basic_vector2<value_type>& vector) noexcept;

  // -- Assignment operators --

  /**
   * @brief Copies the components from the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector2<value_type>& A reference to this vector.
   */
  constexpr basic_vector2<value_type>& operator=(const basic_vector2<value_type>&) noexcept = default;

  /**
   * @brief Copies the components from the other vector.
   * 
   * @tparam From The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector2<value_type>& A reference to this vector.
   */
  template<typename From>
  constexpr basic_vector2<value_type>& operator=(const basic_vector2<From>& other) noexcept;

  /**
   * @brief Moves the components out of the other vector.
   * 
   * @param other The other vector to move the components from.
   * 
   * @return basic_vector2<value_type>& A reference to this vector.
   */
  constexpr basic_vector2<value_type>& operator=(basic_vector2<value_type>&&) noexcept = default;

  // -- Unary arithmetic operators --

  /**
   * @brief Negates the vector.
   * 
   * @return basic_vector2<value_type> A reference to this vector.
   */
  constexpr basic_vector2<value_type>& operator-() noexcept;

  // -- Binary arithmetic operators --

  /**
   * @brief Adds the components of the other vector to this vector.
   * 
   * @param other The other vector to add.
   * 
   * @return basic_vector2<value_type>& A reference to this vector. 
   */
  constexpr basic_vector2<value_type>& operator+=(const basic_vector2<value_type>& other) noexcept;

  /**
   * @brief Subtracts the components of the other vector from this vector.
   * 
   * @param other The other vector to subtract.
   * 
   * @return basic_vector2<value_type>& A reference to this vector. 
   */
  constexpr basic_vector2<value_type>& operator-=(const basic_vector2<value_type>& other) noexcept;

  /**
   * @brief Multiplies the components of this vector by the scalar.
   * 
   * @param scalar The scalar to multiply by.
   * 
   * @return basic_vector2<value_type>& A reference to this vector. 
   */
  constexpr basic_vector2<value_type>& operator*=(const value_type scalar) noexcept;

  /**
   * @brief Divides the components of this vector by the scalar.
   * 
   * @param scalar The scalar to divide by.
   * 
   * @throws std::domain_error If the scalar is zero.
   * 
   * @return basic_vector2<value_type>& A reference to this vector. 
   */
  constexpr basic_vector2<value_type>& operator/=(const value_type scalar);

  // -- Member functions --

  /**
   * @brief Returns the length of the vector.
   * 
   * @return value_type The length of the vector.
   */
  [[nodiscard]] constexpr length_type length() const noexcept;

  /** @brief Normalizes the vector. */
  constexpr void normalize() noexcept;

}; // struct basic_vector2

// Free comparison operators

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
template<typename Type>
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
template<typename Type>
[[nodiscard]] constexpr bool operator!=(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept;

// Free arithmetic operators

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
template<typename Type>
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
template<typename Type>
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
template<typename Type>
[[nodiscard]] constexpr basic_vector2<Type> operator*(basic_vector2<Type> lhs, const Type rhs) noexcept;

/**
 * @brief Devides a vector by a scalar.
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
template<typename Type>
[[nodiscard]] constexpr basic_vector2<Type> operator/(basic_vector2<Type> lhs, const Type rhs) noexcept;

// Free stream operators

/**
 * @brief Writes a vector to a output stream.
 * 
 * @tparam OutputStream The type of the output stream.
 * @tparam Type The type of the vectors components.
 * 
 * @param output_stream The output stream to write to.
 * @param vector The vector to write.
 * 
 * @return OutputStream& A Reference to the output stream.
 */
template<typename OutputStream, typename Type>
constexpr OutputStream& operator<<(OutputStream& output_stream, const basic_vector2<Type>& vector) noexcept;

/**
 * @brief Reads a vector from a input stream. 
 * 
 * @tparam InputStream The type of the input stream.
 * @tparam Type The type of the vectors components.
 * 
 * @param input_stream The input stream to read from.
 * @param vector The vector to read into.
 * 
 * @return InputStream& A reference to the input stream. 
 */
template<typename InputStream, typename Type>
constexpr InputStream& operator>>(InputStream& input_stream, basic_vector2<Type>& vector) noexcept;

// -- Type aliases --

/** @brief Type alias for a two-dimensional vector with 32 bit floating-point components. */
using vector2f = basic_vector2<float32>;

/** @brief Type alias for a two-dimensional vector with 32 bit integer components. */
using vector2i = basic_vector2<int32>;

/** @brief Type alias for vector2f. */
using vector2 = vector2f;

} // namespace sbx

#include "vector2_inl.hpp"

#endif // SBX_MATH_VECTOR2_HPP_
