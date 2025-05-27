#ifndef LIBSBX_MATH_BOX_HPP_
#define LIBSBX_MATH_BOX_HPP_

#include <array>

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/plane.hpp>
#include <libsbx/math/vector3.hpp>

namespace sbx::math {

template<scalar Type>
class basic_box {

public:

  using value_type = Type;
  using plane_type = basic_plane<value_type>;
  using size_type = std::size_t;

  basic_box() noexcept = default;

  basic_box(const std::array<plane_type, 6u>& planes) noexcept
  : _planes{planes} { }

  basic_box(std::array<plane_type, 6u>&& planes) noexcept
  : _planes{std::move(planes)} { }

  auto planes() const noexcept -> const std::array<plane_type, 6u>& {
    return _planes;
  }

  auto plane(const size_type index) const noexcept -> const plane_type& {
    return _planes[index];
  }

private:

  std::array<plane_type, 6u> _planes;

}; // class basic_box

using boxf = basic_box<std::float_t>;

using box = boxf;

} // namespace sbx::math

#endif // LIBSBX_MATH_BOX_HPP_
