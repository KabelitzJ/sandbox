#ifndef SBX_MATH_MATRIX4X4_HPP_
#define SBX_MATH_MATRIX4X4_HPP_

#include <array>
#include <cstddef>
#include <fstream>
#include <ostream>
#include <type_traits>

#include "vector4.hpp"

namespace sbx {

template<typename Type>
struct basic_matrix4x4 {

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

  /** @brief Type of the matrix columns */
  using column_type = basic_vector4<value_type>;

  /** @brief Type reference type of the matrix columns */
  using column_type_reference = column_type&;

  /** @brief The const reference type of the matrix columns. */
  using const_column_type_reference = const column_type&;

  // -- Static data members --

  /** @brief The identity matrix. */
  inline static constexpr basic_matrix4x4<value_type> identity{
    value_type{1}, value_type{0}, value_type{0}, value_type{0},
    value_type{0}, value_type{1}, value_type{0}, value_type{0},
    value_type{0}, value_type{0}, value_type{1}, value_type{0},
    value_type{0}, value_type{0}, value_type{0}, value_type{1}
  };

  /** @brief A Matrix with all components set to zero. */
  inline static constexpr basic_matrix4x4<value_type> zero{
    value_type{0}, value_type{0}, value_type{0}, value_type{0},
    value_type{0}, value_type{0}, value_type{0}, value_type{0},
    value_type{0}, value_type{0}, value_type{0}, value_type{0},
    value_type{0}, value_type{0}, value_type{0}, value_type{0}
  };

  // -- Constructors --

  /** @brief Constructs a matrix with all components set to zero. */
  constexpr basic_matrix4x4() noexcept;

  /**
   * @brief Constructs a matrix with the given columns. 
   * 
   * @param column The columns of the matrix.
   */
  explicit constexpr basic_matrix4x4(const std::array<column_type, 4>& columns) noexcept;

  /**
   * @brief Constructs a matrix with the given columns.

   * @param column_0 The first column of the matrix. 
   * @param column_1 The second column of the matrix.
   * @param column_2 The third column of the matrix. 
   * @param column_3 The fourth column of the matrix.
   */
  constexpr basic_matrix4x4(
    const column_type& column_0,
    const column_type& column_1,
    const column_type& column_2,
    const column_type& column_3
  ) noexcept;

  /**
   * @brief Constructs a matrix with the given values. 
   * 
   * @param x1 The x value of the first column.
   * @param y1 The y value of the first column.
   * @param z1 The z value of the first column.
   * @param w1 The w value of the first column.
   * @param x2 The x value of the second column.
   * @param y2 The y value of the second column.
   * @param z2 The z value of the second column.
   * @param w2 The w value of the second column.
   * @param x3 The x value of the third column.
   * @param y3 The y value of the third column.
   * @param z3 The z value of the third column.
   * @param w3 The w value of the third column.
   * @param x4 The x value of the fourth column.
   * @param y4 The y value of the fourth column.
   * @param z4 The z value of the fourth column.
   * @param w4 The w value of the fourth column.
   */
  constexpr basic_matrix4x4(
    const value_type x1, const value_type y1, const value_type z1, const value_type w1,
    const value_type x2, const value_type y2, const value_type z2, const value_type w2,
    const value_type x3, const value_type y3, const value_type z3, const value_type w3,
    const value_type x4, const value_type y4, const value_type z4, const value_type w4
  ) noexcept;

  /**
   * @brief Copies the values of the given matrix.
   * 
   * @param other The matrix to copy.
   */
  constexpr basic_matrix4x4(const basic_matrix4x4<value_type>& other) noexcept = default;

  /**
   * @brief Copies the values of the given matrix.
   * 
   * @param other The matrix to copy.
   */
  template<typename From>
  explicit constexpr basic_matrix4x4(const basic_matrix4x4<From>& other) noexcept;

  /**
   * @brief Moves the values out of the given matrix.
   * 
   * @param other The matrix to move from.
   */
  constexpr basic_matrix4x4(basic_matrix4x4<value_type>&& other) noexcept = default;

  /** @brief Destroys the matrix */
  ~basic_matrix4x4() noexcept = default;

  // -- Assignment operators --

  /**
   * @brief Copies the values of the given matrix.
   * 
   * @param other The matrix to copy.
   * 
   * @return basic_matrix4x4<value_type>& Reference to this matrix.
   */
  constexpr basic_matrix4x4<value_type>& operator=(const basic_matrix4x4<value_type>& other) noexcept = default;

  /**
   * @brief Copies the values of the given matrix.
   *
   * @param other The matrix to copy. 
   * 
   * @return basic_matrix4x4<value_type>& Reference to this matrix.
   */
  template<typename From>
  constexpr basic_matrix4x4<value_type>& operator=(const basic_matrix4x4<From>& other) noexcept;

  /**
   * @brief Moves the values out of the given matrix.
   *
   * @param other The matrix to move from. 
   * 
   * @return basic_matrix4x4<value_type>& Reference to this matrix.
   */
  constexpr basic_matrix4x4<value_type>& operator=(basic_matrix4x4<value_type>&& other) noexcept = default;

  // -- Access operators --

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component.
   * 
   * @return reference A reference to the component. 
   */
  [[nodiscard]] constexpr column_type_reference operator[](const index_type index) noexcept;

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component. 
   * 
   * @return const_reference A const reference to the component.
   */
  [[nodiscard]] constexpr const_column_type_reference operator[](const index_type index) const noexcept;

  // -- Data access --

  /**
   * @brief Return a pointer to the first component of the matrix.
   *
   * @return pointer A pointer to the first component of the matrix. 
   */
  [[nodiscard]] constexpr pointer data() noexcept;

  /**
   * @brief Return a pointer to the first component of the matrix.
   * 
   * @return const_pointer A pointer to the first component of the matrix.
   */
  [[nodiscard]] constexpr const_pointer data() const noexcept;

private:

  // -- Data members --

  std::array<column_type, 4> _columns{};

}; // struct 

// -- Free comparison operators --

template<typename Type>
[[nodiscard]] constexpr bool operator==(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept;

template<typename Type>
[[nodiscard]] constexpr bool operator!=(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept;

// -- Free arythmetic operators --

template<typename Type>
[[nodiscard]] constexpr basic_vector4<Type> operator*(basic_matrix4x4<Type> lhs, const basic_vector4<Type>& rhs) noexcept;

template<typename Type>
[[nodiscard]] constexpr basic_matrix4x4<Type> operator*(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept;

// -- Free stream operators --

template<typename Type>
constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_matrix4x4<Type>& matrix) noexcept;

// -- Type aliases --

/** @brief Type alias for a four by four matrix with 32 bit floating-point components. */
using matrix4x4f = basic_matrix4x4<float32>;

/** @brief Type alias for a four by four matrix with 32 bit integer components. */
using matrix4x4i = basic_matrix4x4<int32>;

/** @brief Type alias for matrix4x4f. */
using matrix4x4 = matrix4x4f;

} // namespace sbx

#include "matrix4x4.inl"

#endif // SBX_MATH_MATRIX4X4_HPP_
