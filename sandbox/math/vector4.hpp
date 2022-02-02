#ifndef SBX_MATH_VECTOR4_HPP_
#define SBX_MATH_VECTOR4_HPP_

#include <fstream>
#include <ostream>
#include <type_traits>

#include <types/primitives.hpp>

#include "vector3.hpp"

namespace sbx {

template<typename Type>
struct basic_vector4 {

  // Vector components can only be arithmetic types.
  static_assert(std::is_arithmetic_v<Type>, "Type must be arithmetic");

  // -- Type aliases --

  /** @brief The type of the vector components. */
  using value_type = Type;

  /** @brief The reference type of the vector components. */
  using reference = Type&;

  /** @brief The const reference type of the vector components. */
  using const_reference = const Type&;

  /** @brief The pointer type of the vector components. */
  using pointer = Type*;

  /** @brief The const pointer type of the vector components. */
  using const_pointer = const Type*;

  /** @brief The type that can describe the length of the vector */
  using length_type = float32;

  /** @brief The type that can index compotents */
  using index_type = std::size_t; 

  // -- Static data members --

  /** @brief The origin of three dimensional space */
  inline static constexpr basic_vector4<value_type> origin{value_type{0}, value_type{0}, value_type{0}, value_type{0}};

  // -- Data members --

  /** @brief The x-component of the vector. */
  value_type x{};
  /** @brief The y-component of the vector. */
  value_type y{};
  /** @brief The z-component of the vector. */
  value_type z{};
  /** @brief The w-component of the vector. */
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
   * @param w The value for the w component.
   */
  constexpr basic_vector4(const basic_vector3<value_type>& vector, const value_type w = value_type{1}) noexcept;

  /** 
   * @brief Constructs a vector and copies the components from the other vector
   *
   * @param other The other vector to copy the components from. 
   */
  constexpr basic_vector4(const basic_vector4<value_type>&) noexcept = default;

  /**
   * @brief Constructs a vector and copies the components from the other vector
   * 
   * @tparam From The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   */
  template<typename From>
  constexpr basic_vector4(const basic_vector4<From>& other) noexcept;

  /** 
   * @brief Constructs a vector and moves the components out of the other vector
   *
   * @param other The other vector to move the components from. 
   */
  constexpr basic_vector4(basic_vector4<value_type>&&) noexcept = default;

  /** @brief Destroys the vector */
  ~basic_vector4() noexcept = default;

  // -- Static member functions --

  /**
   * @brief Returns a normalized copy of the vector.
   * 
   * @return basic_vector4<value_type> The normalized vector.
   */
  [[nodiscard]] static constexpr basic_vector4<value_type> normalized(const basic_vector4<value_type>& vector) noexcept;

  // -- Assignment operators --

  /**
   * @brief Copies the components from the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector4<value_type>& A reference to this vector.
   */
  constexpr basic_vector4<value_type>& operator=(const basic_vector4<value_type>&) noexcept = default;

  /**
   * @brief Copies the components from the other vector.
   * 
   * @tparam From The type of the other vector.
   * 
   * @param other The other vector to copy the components from.
   * 
   * @return basic_vector4<value_type>& A reference to this vector.
   */
  template<typename From>
  constexpr basic_vector4<value_type>& operator=(const basic_vector4<From>& other) noexcept;

  /**
   * @brief Moves the components out of the other vector.
   * 
   * @param other The other vector to move the components from.
   * 
   * @return basic_vector4<value_type>& A reference to this vector.
   */
  constexpr basic_vector4<value_type>& operator=(basic_vector4<value_type>&&) noexcept = default;

  // -- Unary arithmetic operators --

  /**
   * @brief Negates the vector.
   * 
   * @return basic_vector4<value_type> A reference to this vector.
   */
  constexpr basic_vector4<value_type>& operator-() noexcept;

  // -- Binary arithmetic operators --

  /**
   * @brief Adds the components of the other vector to this vector.
   * 
   * @param other The other vector to add.
   * 
   * @return basic_vector4<value_type>& A reference to this vector. 
   */
  constexpr basic_vector4<value_type>& operator+=(const basic_vector4<value_type>& other) noexcept;

  /**
   * @brief Subtracts the components of the other vector from this vector.
   * 
   * @param other The other vector to subtract.
   * 
   * @return basic_vector4<value_type>& A reference to this vector. 
   */
  constexpr basic_vector4<value_type>& operator-=(const basic_vector4<value_type>& other) noexcept;

  /**
   * @brief Multiplies the components of this vector by the scalar.
   * 
   * @param scalar The scalar to multiply by.
   * 
   * @return basic_vector4<value_type>& A reference to this vector. 
   */
  constexpr basic_vector4<value_type>& operator*=(const value_type scalar) noexcept;

  /**
   * @brief Divides the components of this vector by the scalar.
   * 
   * @param scalar The scalar to divide by.
   * 
   * @throws std::domain_error If the scalar is zero.
   * 
   * @return basic_vector4<value_type>& A reference to this vector. 
   */
  constexpr basic_vector4<value_type>& operator/=(const value_type scalar);

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
   * @brief Returns the length of the vector.
   * 
   * @return value_type The length of the vector.
   */
  [[nodiscard]] constexpr length_type length() const noexcept;

  /** @brief Normalizes the vector. */
  constexpr void normalize() noexcept;

  /**
   * @brief Return a pointer to the first component of the vector.
   *
   * @return pointer A pointer to the first component of the vector. 
   */
  [[nodiscard]] constexpr pointer data() noexcept;

  /**
   * @brief Return a pointer to the first component of the vector.
   * 
   * @return const_pointer A pointer to the first component of the vector.
   */
  [[nodiscard]] constexpr const_pointer data() const noexcept;

}; // struct basic_vector4

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
[[nodiscard]] constexpr bool operator==(const basic_vector4<Type>& lhs, const basic_vector4<Type>& rhs) noexcept;

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
[[nodiscard]] constexpr bool operator!=(const basic_vector4<Type>& lhs, const basic_vector4<Type>& rhs) noexcept;

// Free arithmetic operators

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
template<typename Type>
[[nodiscard]] constexpr basic_vector4<Type> operator+(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept;

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
template<typename Type>
[[nodiscard]] constexpr basic_vector4<Type> operator-(basic_vector4<Type> lhs, const basic_vector4<Type>& rhs) noexcept;

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
template<typename Type>
[[nodiscard]] constexpr basic_vector4<Type> operator*(basic_vector4<Type> lhs, const Type rhs) noexcept;

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
 * @return basic_vector4<Type> The quotient of the vector and scalar.
 */
template<typename Type>
[[nodiscard]] constexpr basic_vector4<Type> operator/(basic_vector4<Type> lhs, const Type rhs);

// Free stream operators

/**
 * @brief Writes a vector to a output stream.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param output_stream The output stream to write to.
 * @param vector The vector to write.
 * 
 * @return std::ostream& A Reference to the output stream.
 */
template<typename Type>
constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_vector4<Type>& vector);

/**
 * @brief Writes a vector to a file output stream.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param output_stream The output stream to write to.
 * @param vector The vector to write.
 * 
 * @return std::ostream& A Reference to the output stream.
 */
template<typename Type>
constexpr std::ofstream& operator<<(std::ofstream& output_stream, const basic_vector4<Type>& vector);

/**
 * @brief Writes a vector to a generic output stream.
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
constexpr OutputStream& operator<<(OutputStream& output_stream, const basic_vector4<Type>& vector);


/**
 * @brief Reads a vector from a file input stream.
 * 
 * @tparam Type The type of the vectors components.
 * 
 * @param input_stream The input stream to read from.
 * @param vector The vector to read.
 * 
 * @return std::ifstream& A reference to the input stream.
 */
template<typename Type>
constexpr std::ifstream& operator>>(std::ifstream& input_stream, basic_vector4<Type>& vector);

/**
 * @brief Reads a vector from a generic input stream.
 * 
 * @tparam InputStream The type of the input stream.
 * @tparam Type The type of the vectors components.
 * 
 * @param input_stream The input stream to read from.
 * @param vector The vector to read.
 * 
 * @return InputStream& A reference to the input stream.
 */
template<typename InputStream, typename Type>
constexpr InputStream& operator>>(InputStream& input_stream, basic_vector4<Type>& vector);

// -- Type aliases --

/** @brief Type alias for a four-dimensional vector with 32 bit floating-point components. */
using vector4f = basic_vector4<float32>;

/** @brief Type alias for a four-dimensional vector with 32 bit integer components. */
using vector4i = basic_vector4<int32>;

/** @brief Type alias for vector2f. */
using vector4 = vector4f;

} // namespace sbx

#include "vector4_inl.hpp"

#endif // SBX_MATH_VECTOR4_HPP_
