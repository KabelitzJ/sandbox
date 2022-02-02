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
   * @brief Constructs a matrix with the given values
   * 
   * @param values The values of the matrix.
   */
  explicit constexpr basic_matrix4x4(std::initializer_list<value_type> values) noexcept;

  /** @brief Destroys the matrix */
  ~basic_matrix4x4() noexcept = default;

  // -- Static member functions --

  /**
   * @brief Generates a new identity matrix.
   * 
   * @return basic_matrix4x4<Type> The identity matrix. 
   */
  [[nodiscard]] static constexpr basic_matrix4x4<value_type> identity() noexcept;

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
