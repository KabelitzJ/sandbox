#ifndef LIBSBX_MATH_VOLUME_HPP_
#define LIBSBX_MATH_VOLUME_HPP_

#include <libsbx/math/concepts.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>

namespace sbx::math {

template<scalar Type>
class basic_volume {

public:

  using value_type = Type;
  using vector_type = basic_vector3<value_type>;

  basic_volume() noexcept = default;
  
  basic_volume(const vector_type& min, const vector_type& max) noexcept
  : _min{min}, 
    _max{max} { }

  static auto transformed(const basic_volume& volume, const math::matrix4x4& matrix) -> basic_volume {
    auto min = math::vector3{std::numeric_limits<std::float_t>::max()};
    auto max = math::vector3{std::numeric_limits<std::float_t>::lowest()};

    for (const auto& corner : volume.corners()) {
      const auto transformed = math::vector3{matrix * math::vector4{corner, 1.0f}};
      min = math::vector3::min(min, transformed);
      max = math::vector3::max(max, transformed);
    }

    return basic_volume{min, max};
  }

  auto min() const noexcept -> const vector_type& {
    return _min;
  }

  auto max() const noexcept -> const vector_type& {
    return _max;
  }

  auto center() const noexcept -> vector_type {
    return (_min + _max) / 2.0f;
  }

  auto corners() const noexcept -> std::array<math::vector3, 8u> {
    return std::array<math::vector3, 8u>{
      math::vector3{_min.x(), _min.y(), _min.z()},
      math::vector3{_min.x(), _min.y(), _max.z()},
      math::vector3{_min.x(), _max.y(), _min.z()},
      math::vector3{_min.x(), _max.y(), _max.z()},
      math::vector3{_max.x(), _min.y(), _min.z()},
      math::vector3{_max.x(), _min.y(), _max.z()},
      math::vector3{_max.x(), _max.y(), _min.z()},
      math::vector3{_max.x(), _max.y(), _max.z()}
    };
  }

  auto contains(const vector_type& point) const noexcept -> bool {
    return point.x() >= _min.x() && point.x() <= _max.x() && point.y() >= _min.y() && point.y() <= _max.y() && point.z() >= _min.z() && point.z() <= _max.z();
  }

  auto contains(const basic_volume& other) const noexcept -> bool {
    return _min.x() <= other.min().x() && _min.y() <= other.min().y() && _min.z() <= other.min().z() && _max.x() >= other.max().x() && _max.y() >= other.max().y() && _max.z() >= other.max().z();
  }

  auto intersects(const basic_volume& other) const noexcept -> bool {
    return _min.x() <= other.max().x() && _max.x() >= other.min().x() && _min.y() <= other.max().y() && _max.y() >= other.min().y() && _min.z() <= other.max().z() && _max.z() >= other.min().z();
  }

private:

  vector_type _min;
  vector_type _max;

}; // class basic_volume

using volumef = basic_volume<std::float_t>;

using volume = volumef;

} // namespace sbx::math

#endif // LIBSBX_MATH_VOLUME_HPP_
