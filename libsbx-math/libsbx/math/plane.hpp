#ifndef LIBSBX_MATH_PLANE_HPP_
#define LIBSBX_MATH_PLANE_HPP_

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector3.hpp>

namespace sbx::math {

template<scalar Type>
class basic_plane {

public:

  using value_type = Type;
  using vector_type = basic_vector3<value_type>;

  basic_plane() noexcept = default;

  basic_plane(const vector_type& normal, const value_type distance) noexcept
  : _normal{normal}, 
    _distance{distance} { }

  auto normal() const noexcept -> const vector_type& {
    return _normal;
  }

  auto distance() const noexcept -> value_type {
    return _distance;
  }

  auto distance_to_point(const vector_type& point) const noexcept -> value_type {
    return math::vector3::dot(_normal, point) + _distance;
  }

  auto normalize() noexcept -> basic_plane& {
    const auto length = _normal.length();
    
    _normal /= length;
    _distance /= length;

    return *this;
  }

private:

  vector_type _normal;
  value_type _distance;

}; // class basic_plane

using planef = basic_plane<std::float_t>;

using plane = planef;

} // namespace sbx::math

#endif // LIBSBX_MATH_PLANE_HPP_
