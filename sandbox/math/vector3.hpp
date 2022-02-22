#ifndef SBX_MATH_VECTOR3_HPP_
#define SBX_MATH_VECTOR3_HPP_

#include <cstddef>
#include <concepts>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <meta/concepts.hpp>

#include <types/primitives.hpp>

#include "vector2.hpp"

namespace sbx {

/**
 * @brief A vector in three-dimensional space.
 *
 * @tparam Type The type of the vectors components.
 */
template<arithmetic Type>
struct basic_vector3 {

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
  using length_type = float32;

  /** @brief The type that can index compotents */
  using index_type = std::size_t;

  // -- Static data members --

  /** @brief A vector with all components set to zero. */
  inline static constexpr basic_vector3 zero{value_type{0}, value_type{0}, value_type{0}};

  /** @brief A unit vector along the positive x-axis */
  inline static constexpr basic_vector3 right{value_type{1}, value_type{0}, value_type{0}};

  /** @brief A unit vector along the negative x-axis */
  inline static constexpr basic_vector3 left{value_type{-1}, value_type{0}, value_type{0}};

  /** @brief A unit vector along the positive y-axis */
  inline static constexpr basic_vector3 up{value_type{0}, value_type{1}, value_type{0}};

  /** @brief A unit vector along the negative y-axis */
  inline static constexpr basic_vector3 down{value_type{0}, value_type{-1}, value_type{0}};

  /** @brief A unit vector along the positive z-axis */
  inline static constexpr basic_vector3 backward{value_type{0}, value_type{0}, value_type{1}};

  /** @brief A unit vector along the negative z-axis */
  inline static constexpr basic_vector3 forward{value_type{0}, value_type{0}, value_type{-1}};

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
  explicit constexpr basic_vector3(const basic_vector2<value_type>& vector, const value_type z = value_type{1}) noexcept;

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
   * @return basic_vector3<value_type> The normalized vector.
   */
  [[nodiscard]] static constexpr basic_vector3<value_type> normalized(const basic_vector3& vector) noexcept;

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
   * @return basic_vector3<value_type> The cross product.
   */
  [[nodiscard]] static constexpr basic_vector3<value_type> cross(const basic_vector3& lhs, const basic_vector3& rhs) noexcept;

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
   * @return basic_vector3<value_type> A reference to this vector.
   */
  constexpr basic_vector3& operator-() noexcept;

  // -- Binary arithmetic operators --

  /**
   * @brief Adds the components of the other vector to this vector.
   * 
   * @param other The other vector to add.
   * 
   * @return basic_vector3& A reference to this vector. 
   */
  constexpr basic_vector3& operator+=(const basic_vector3& other) noexcept;

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
  constexpr void normalize() noexcept;

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

}; // struct vector3

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
 * @return basic_vector3<Type> The quotient of the vector and scalar.
 */
template<arithmetic Type>
[[nodiscard]] constexpr basic_vector3<Type> operator/(basic_vector3<Type> lhs, const Type rhs);

// -- Free stream operators --

/**
 * @brief Writes a vector to a output stream.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param output_stream The output stream to write to.
 * @param vector The vector to write.
 * 
 * @return OutputStream& A Reference to the output stream.
 */
template<arithmetic Type>
constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_vector3<Type>& vector);

/**
 * @brief Writes a vector to a file output stream.
 * 
 * @tparam Type The type of the vectors components. 
 * 
 * @param output_stream The output stream to write to. 
 * @param vector The vector to write.
 * 
 * @return std::ofstream& A reference to the output stream.
 */
template<arithmetic Type>
constexpr std::ofstream& operator<<(std::ofstream& output_stream, const basic_vector3<Type>& vector);

/**
 * @brief Writes a vector to a generic output stream.
 * 
 * @tparam OutputStream The type of the output stream. 
 * @tparam Type The type of the vectors components.
 * 
 * @param output_stream The output stream to write to. 
 * @param vector The vector to write.
 * 
 * @return OutputStream& A reference to the output stream.
 */
template<arithmetic Type, output_stream<Type> OutputStream>
constexpr OutputStream& operator<<(OutputStream& output_stream, const basic_vector3<Type>& vector);

/**
 * @brief Reads a vector from a file input stream.
 * 
 * @tparam Type The type of the vectors components. 
 * 
 * @param input_stream The input stream to read from.
 * @param vector The vector to read into.
 * 
 * @return std::istream& A reference to the input stream. 
 */
template<arithmetic Type>
constexpr std::istream& operator>>(std::istream& input_stream, basic_vector3<Type>& vector);

/**
 * @brief Reads a vector from a generic input stream.
 * 
 * @tparam InputStream The type of the input stream.
 * @tparam Type The type of the vectors components.
 * 
 * @param input_stream The input stream to read from.
 * @param vector The vector to read into.
 * 
 * @return InputStream& A reference to the input stream.
 */
template<arithmetic Type, input_stream<Type> InputStream>
constexpr InputStream& operator>>(InputStream& input_stream, basic_vector3<Type>& vector);

// -- Type aliases --

/** @brief Type alias for a three dimensional vector with 32 bit floating-point components. */
using vector3f = basic_vector3<float32>;

/** @brief Type alias for a three dimensional vector with 32 bit integer components. */
using vector3i = basic_vector3<int32>;

/** @brief Type alias for vector3f. */
using vector3 = vector3f;

} // namespace sbx

#include "vector3.inl"

#endif // SBX_MATH_VECTOR3_HPP_
