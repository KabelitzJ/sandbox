#ifndef LIBSBX_MATH_MATRIX_HPP_
#define LIBSBX_MATH_MATRIX_HPP_

#include <array>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector.hpp>

namespace sbx::math {

template<std::size_t Columns, std::size_t Rows, scalar Type>
requires (Columns > 1u && Rows > 1u)
class basic_matrix {

public:

  enum class direction : std::uint8_t {
    column = 0u,
    row = 1u
  }; // enum class direction

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = std::size_t;
  using column_type = basic_vector<Rows, Type>;

  template<scalar Other = value_type>
  constexpr basic_matrix(Other value = Other{0}) noexcept;

  template<scalar Other = value_type>
  constexpr basic_matrix(const basic_matrix<Columns, Rows, Other>& other) noexcept;
  
  [[nodiscard]] constexpr auto operator[](size_type index) noexcept -> column_type&;

  [[nodiscard]] constexpr auto operator[](size_type index) const noexcept -> const column_type&;

  template<scalar Other>
  constexpr auto operator+=(const basic_matrix<Columns, Rows, Other>& other) noexcept -> basic_matrix&;

  template<scalar Other>
  constexpr auto operator-=(const basic_matrix<Columns, Rows, Other>& other) noexcept -> basic_matrix&;

  template<scalar Other>
  constexpr auto operator*=(Other scalar) noexcept -> basic_matrix&;

  template<scalar Other>
  constexpr auto operator/=(Other scalar) noexcept -> basic_matrix&;

  constexpr auto row(const size_type row) const noexcept -> basic_vector<Columns, value_type>;

protected:

  template<scalar Other>
  using column_type_for = basic_vector<Rows, Other>;

  template<typename... Args>
  constexpr basic_matrix(Args&&... args) noexcept;

  constexpr static auto identity() noexcept -> basic_matrix;

private:

  std::array<column_type, Columns> _columns;

}; // class basic_matrix

template<std::size_t Columns, std::size_t Rows, scalar Lhs, scalar Rhs>
[[nodiscard]] constexpr auto operator==(const basic_matrix<Columns, Rows, Lhs>& lhs, const basic_matrix<Columns, Rows, Rhs>& rhs) noexcept -> bool;

template<std::size_t Columns, std::size_t Rows, scalar Lhs, scalar Rhs>
[[nodiscard]] constexpr auto operator+(basic_matrix<Columns, Rows, Lhs> lhs, const basic_matrix<Columns, Rows, Rhs>& rhs) noexcept -> basic_matrix<Columns, Rows, Lhs>;

template<std::size_t Columns, std::size_t Rows, scalar Lhs, scalar Rhs>
[[nodiscard]] constexpr auto operator-(basic_matrix<Columns, Rows, Lhs> lhs, const basic_matrix<Columns, Rows, Rhs>& rhs) noexcept -> basic_matrix<Columns, Rows, Lhs>;

template<std::size_t Columns, std::size_t Rows, scalar Lhs, scalar Rhs>
[[nodiscard]] constexpr auto operator*(basic_matrix<Columns, Rows, Lhs> lhs, Rhs rhs) noexcept -> basic_matrix<Columns, Rows, Lhs>;

template<std::size_t Columns, std::size_t Rows, scalar Lhs, scalar Rhs>
[[nodiscard]] constexpr auto operator*(Lhs lhs, basic_matrix<Columns, Rows, Rhs> rhs) noexcept -> basic_matrix<Columns, Rows, Rhs>;

} // namespace sbx::math

#include <libsbx/math/matrix.ipp>

#endif // LIBSBX_MATH_MATRIX_HPP_
