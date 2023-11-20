#ifndef LIBSBX_MATH_MATRIX4X4_HPP_
#define LIBSBX_MATH_MATRIX4X4_HPP_

#include <array>
#include <cstddef>
#include <cmath>
#include <cinttypes>
#include <concepts>
#include <fstream>
#include <ostream>
#include <type_traits>

#include <fmt/format.h>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::math {

/**
 * @brief A 4 by 4 matrix.
 * 
 * @tparam Type The type of the matrix components.
 */
template<numeric Type>
struct basic_matrix4x4 {

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

  /** @brief The type that can index components */
  using index_type = std::size_t;

  /** @brief Type of the matrix columns */
  using column_type = basic_vector4<value_type>;

  /** @brief Type reference type of the matrix columns */
  using column_type_reference = column_type&;

  /** @brief The const reference type of the matrix columns. */
  using const_column_type_reference = const column_type&;

  // -- Static data members --

  /** @brief The identity matrix. */
  inline static constexpr basic_matrix4x4 identity{
    value_type{1}, value_type{0}, value_type{0}, value_type{0},
    value_type{0}, value_type{1}, value_type{0}, value_type{0},
    value_type{0}, value_type{0}, value_type{1}, value_type{0},
    value_type{0}, value_type{0}, value_type{0}, value_type{1}
  };

  /** @brief A Matrix with all components set to zero. */
  inline static constexpr basic_matrix4x4 zero{
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

   * @param column0 The first column of the matrix. 
   * @param column1 The second column of the matrix.
   * @param column2 The third column of the matrix. 
   * @param column3 The fourth column of the matrix.
   */
  constexpr basic_matrix4x4(
    const column_type& column0,
    const column_type& column1,
    const column_type& column2,
    const column_type& column3
  ) noexcept;

  /**
   * @brief Constructs a matrix with the given values. 
   * 
   * @param x0 The x value of the first column.
   * @param x1 The x value of the second column.
   * @param x2 The x value of the third column.
   * @param x3 The x value of the fourth column.
   * @param y0 The y value of the first column.
   * @param y1 The y value of the second column.
   * @param y2 The y value of the third column.
   * @param y3 The y value of the fourth column.
   * @param z0 The z value of the first column.
   * @param z1 The z value of the second column.
   * @param z2 The z value of the third column.
   * @param z3 The z value of the fourth column.
   * @param w0 The w value of the first column.
   * @param w1 The w value of the second column.
   * @param w2 The w value of the third column.
   * @param w3 The w value of the fourth column.
   */
  constexpr basic_matrix4x4(
    const value_type x0, const value_type x1, const value_type x2, const value_type x3,
    const value_type y0, const value_type y1, const value_type y2, const value_type y3,
    const value_type z0, const value_type z1, const value_type z2, const value_type z3,
    const value_type w0, const value_type w1, const value_type w2, const value_type w3
  ) noexcept;

  /**
   * @brief Copies the values of the given matrix.
   * 
   * @param other The matrix to copy.
   */
  constexpr basic_matrix4x4(const basic_matrix4x4& other) noexcept = default;

  /**
   * @brief Copies the values of the given matrix.
   * 
   * @param other The matrix to copy.
   */
  template<numeric From>
  explicit constexpr basic_matrix4x4(const basic_matrix4x4<From>& other) noexcept;

  /**
   * @brief Moves the values out of the given matrix.
   * 
   * @param other The matrix to move from.
   */
  constexpr basic_matrix4x4(basic_matrix4x4&& other) noexcept = default;

  /** @brief Destroys the matrix */
  constexpr ~basic_matrix4x4() noexcept = default;

  // -- Static member functions --

  [[nodiscard]] constexpr static auto transposed(const basic_matrix4x4& matrix) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto inverted(const basic_matrix4x4& matrix) -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto look_at(const basic_vector3<value_type>& position, const basic_vector3<value_type>& target, const basic_vector3<value_type>& up) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static basic_matrix4x4 perspective(const basic_angle<value_type>& fov, const value_type aspect, const value_type near, const value_type far) noexcept;

  [[nodiscard]] constexpr static auto orthographic(const value_type left, const value_type right, const value_type bottom, const value_type top) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto orthographic(const value_type left, const value_type right, const value_type bottom, const value_type top, const value_type near, const value_type far) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto translated(const basic_matrix4x4& matrix, const basic_vector3<value_type>& vector) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static basic_matrix4x4 rotated(const basic_matrix4x4& matrix, const basic_vector3<value_type>& axis, const basic_angle<value_type>& angle) noexcept;

  [[nodiscard]] constexpr static basic_matrix4x4 rotation_from_euler_angles(const basic_vector3<value_type>& euler_angles) noexcept;

  [[nodiscard]] constexpr static auto scaled(const basic_matrix4x4& matrix, const basic_vector3<value_type>& vector) noexcept -> basic_matrix4x4;

  // -- Assignment operators --

  /**
   * @brief Copies the values of the given matrix.
   * 
   * @param other The matrix to copy.
   * 
   * @return basic_matrix4x4& Reference to this matrix.
   */
  constexpr auto operator=(const basic_matrix4x4& other) noexcept -> basic_matrix4x4& = default;

  /**
   * @brief Copies the values of the given matrix.
   *
   * @param other The matrix to copy. 
   * 
   * @return basic_matrix4x4& Reference to this matrix.
   */
  template<numeric From>
  constexpr auto operator=(const basic_matrix4x4<From>& other) noexcept -> basic_matrix4x4&;

  /**
   * @brief Moves the values out of the given matrix.
   *
   * @param other The matrix to move from. 
   * 
   * @return basic_matrix4x4& Reference to this matrix.
   */
  constexpr auto operator=(basic_matrix4x4&& other) noexcept -> basic_matrix4x4& = default;

  // -- Binary numeric operators --

  /**
   * @brief Adds the given matrix to this matrix.
   *
   * @param other The matrix to add. 
   * 
   * @return basic_matrix4x4& Reference to this matrix.
   */
  constexpr auto operator+=(const basic_matrix4x4& other) noexcept -> basic_matrix4x4&;

  /**
   * @brief Subtracts the given matrix from this matrix.
   *
   * @param other The matrix to subtract. 
   * 
   * @return basic_matrix4x4& Reference to this matrix.
   */
  constexpr auto operator-=(const basic_matrix4x4& other) noexcept -> basic_matrix4x4&;

  /**
   * @brief Multiplies this matrix by the given scalar.
   *
   * @param scalar The scalar to multiply by. 
   *
   * @return basic_matrix4x4& Reference to this matrix. 
   */
  constexpr auto operator*=(const value_type scalar) noexcept -> basic_matrix4x4&;

  // -- Access operators --

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component.
   * 
   * @return reference A reference to the component. 
   */
  [[nodiscard]] constexpr auto operator[](const index_type index) -> column_type_reference;

  /**
   * @brief Returns the component at the specified index.
   * 
   * @param index The index of the component. 
   * 
   * @return const_reference A const reference to the component.
   */
  [[nodiscard]] constexpr auto operator[](const index_type index) const -> const_column_type_reference;

  // -- Data access --

  /**
   * @brief Return a pointer to the first component of the matrix.
   *
   * @return pointer A pointer to the first component of the matrix. 
   */
  [[nodiscard]] constexpr auto data() noexcept -> pointer;

  /**
   * @brief Return a pointer to the first component of the matrix.
   * 
   * @return const_pointer A pointer to the first component of the matrix.
   */
  [[nodiscard]] constexpr auto data() const noexcept -> const_pointer;

  // -- Member functions --

  /** @brief Transposes the matrix. */
  constexpr auto transpose() noexcept -> void;

  /** @brief Inverts the matrix. */
  constexpr auto inverse() noexcept -> void;

private:

  // -- Data members --

  std::array<column_type, 4> _columns{};

}; // struct 

// -- Free comparison operators --

template<numeric Type>
[[nodiscard]] constexpr auto operator==(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept -> bool;

// -- Free numeric operators --

template<numeric Type>
[[nodiscard]] constexpr auto operator+(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept -> basic_matrix4x4<Type>;

template<numeric Type>
[[nodiscard]] constexpr auto operator-(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept -> basic_matrix4x4<Type>;

template<numeric Type>
[[nodiscard]] constexpr auto operator*(basic_matrix4x4<Type> lhs, const Type rhs) noexcept -> basic_matrix4x4<Type>;

template<numeric Type>
[[nodiscard]] constexpr auto operator*(basic_matrix4x4<Type> lhs, const basic_vector4<Type>& rhs) noexcept -> basic_vector4<Type>;

template<numeric Type>
[[nodiscard]] constexpr auto operator*(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept -> basic_matrix4x4<Type>;

// -- Type aliases --

/** @brief Type alias for a four by four matrix with 32 bit floating-point components. */
using matrix4x4f = basic_matrix4x4<std::float_t>;

/** @brief Type alias for a four by four matrix with 32 bit integer components. */
using matrix4x4i = basic_matrix4x4<std::int32_t>;

/** @brief Type alias for matrix4x4f. */
using matrix4x4 = matrix4x4f;

} // namespace sbx::math

template<sbx::math::numeric Type>
struct fmt::formatter<sbx::math::basic_matrix4x4<Type>> : formatter<std::string_view> {

  using underlying_formatter_type = formatter<std::string_view>;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& context) -> decltype(context.begin()) {
    return underlying_formatter_type::parse(context);
  }

  template<typename FormatContext>
  auto format(const sbx::math::basic_matrix4x4<Type>& matrix, FormatContext& context) -> decltype(context.out()) {
    return underlying_formatter_type::format(fmt::format(
      "\n{:.2f}, {:.2f}, {:.2f}\n{:.2f}, {:.2f}, {:.2f}\n{:.2f}, {:.2f}, {:.2f}", 
      matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
      matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
      matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
      matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
    ), context);
  }

}; // struct fmt::formatter

#include <libsbx/math/matrix4x4.ipp>

#endif // LIBSBX_MATH_MATRIX4X4_HPP_
