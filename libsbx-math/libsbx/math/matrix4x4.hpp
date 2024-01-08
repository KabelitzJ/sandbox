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
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/matrix.hpp>

namespace sbx::math {

template<scalar Type>
class basic_matrix4x4 : public basic_matrix<4u, 4u, Type> {

  using base_type = basic_matrix<4u, 4u, Type>;

  template<scalar Other>
  using column_type_for = basic_vector4<Other>;

  inline static constexpr auto x_axis = std::size_t{0u};
  inline static constexpr auto y_axis = std::size_t{1u};
  inline static constexpr auto z_axis = std::size_t{2u};
  inline static constexpr auto w_axis = std::size_t{3u};

public:

  using value_type = base_type::value_type;
  using reference = base_type::reference;
  using const_reference = base_type::const_reference;
  using size_type = base_type::size_type;
  using column_type = column_type_for<value_type>;

  inline static constexpr basic_matrix4x4 identity{base_type::identity()};

  inline static constexpr basic_matrix4x4 zero{base_type{value_type{0}}};

  using base_type::base_type;

  constexpr basic_matrix4x4(const base_type& base) noexcept;

  template<scalar Other>
  constexpr basic_matrix4x4(
    const column_type_for<Other>& column0,
    const column_type_for<Other>& column1,
    const column_type_for<Other>& column2,
    const column_type_for<Other>& column3
  ) noexcept;

  template<scalar Other>
  constexpr basic_matrix4x4(
    Other x0, Other x1, Other x2, Other x3,
    Other y0, Other y1, Other y2, Other y3,
    Other z0, Other z1, Other z2, Other z3,
    Other w0, Other w1, Other w2, Other w3
  ) noexcept;

//   // -- Static member functions --

  [[nodiscard]] constexpr static auto transposed(const basic_matrix4x4& matrix) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto inverted(const basic_matrix4x4& matrix) -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto look_at(const basic_vector3<value_type>& position, const basic_vector3<value_type>& target, const basic_vector3<value_type>& up) noexcept -> basic_matrix4x4;

  // [[nodiscard]] constexpr static auto perspective(const basic_angle<value_type>& fov, const value_type aspect, const value_type near, const value_type far) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto orthographic(const value_type left, const value_type right, const value_type bottom, const value_type top) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto orthographic(const value_type left, const value_type right, const value_type bottom, const value_type top, const value_type near, const value_type far) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto translated(const basic_matrix4x4& matrix, const basic_vector3<value_type>& vector) noexcept -> basic_matrix4x4;

  [[nodiscard]] constexpr static auto scaled(const basic_matrix4x4& matrix, const basic_vector3<value_type>& vector) noexcept -> basic_matrix4x4;

  // [[nodiscard]] constexpr static auto rotated(const basic_matrix4x4& matrix, const basic_vector3<value_type>& axis, const basic_angle<value_type>& angle) noexcept -> basic_matrix4x4;

  // [[nodiscard]] constexpr static auto rotation_from_euler_angles(const basic_vector3<value_type>& euler_angles) noexcept -> basic_matrix4x4;

}; // class basic_matrix4x4

using matrix4x4f = basic_matrix4x4<std::float_t>;

using matrix4x4i = basic_matrix4x4<std::int32_t>;

using matrix4x4 = matrix4x4f;

} // namespace sbx::math

template<sbx::math::scalar Type>
struct fmt::formatter<sbx::math::basic_matrix4x4<Type>> {

  template<typename ParseContext>
  constexpr auto parse(ParseContext& context) -> decltype(context.begin()) {
    return context.begin();
  }

  template<typename FormatContext>
  auto format(const sbx::math::basic_matrix4x4<Type>& matrix, FormatContext& context) -> decltype(context.out()) {
    if constexpr (sbx::math::is_floating_point_v<Type>) {
      return fmt::format_to(context.out(), 
        "\n{:.2f}, {:.2f}, {:.2f}\n{:.2f}, {:.2f}, {:.2f}\n{:.2f}, {:.2f}, {:.2f}", 
        matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
        matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
        matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
        matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
      );
    } else {
      return fmt::format_to(context.out(), 
        "\n{}, {}, {}\n{}, {}, {}\n{}, {}, {}", 
        matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
        matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
        matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
        matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
      );
    }
  }

}; // struct fmt::formatter

#include <libsbx/math/matrix4x4.ipp>

#endif // LIBSBX_MATH_MATRIX4X4_HPP_
