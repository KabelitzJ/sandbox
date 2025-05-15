#ifndef LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
#define LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_

#include <variant>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/volume.hpp>
#include <libsbx/math/sphere.hpp>
#include <libsbx/math/plane.hpp>

#include <libsbx/containers/static_vector.hpp>

namespace sbx::scenes {

using aabb_collider = math::volume;
using sphere_collider = math::sphere;

using collider = std::variant<aabb_collider, sphere_collider>;

class frustum {

  inline static constexpr auto left_plane = std::size_t{0u};
  inline static constexpr auto right_plane = std::size_t{0u};
  inline static constexpr auto top_plane = std::size_t{1u};
  inline static constexpr auto bottom_plane = std::size_t{1u};
  inline static constexpr auto near_plane = std::size_t{2u};
  inline static constexpr auto far_plane = std::size_t{2u};

public:

  frustum(const math::matrix4x4& view_projection) {
    const auto& m = view_projection;

    // Right
    {
      const auto normal = math::vector3{m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0]};
      const auto distance = m[3][3] - m[3][0];
      _planes.push_back(math::plane{normal, distance}.normalize());
    }

    // Left
    {
      const auto normal = math::vector3{m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0]};
      const auto distance = m[3][3] + m[3][0];
      _planes.push_back(math::plane{normal, distance}.normalize());
    }

    // Bottom
    {
      const auto normal = math::vector3{m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1]};
      const auto distance = m[3][3] + m[3][1];
      _planes.push_back(math::plane{normal, distance}.normalize());
    }

    // Top
    {
      const auto normal = math::vector3{m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1]};
      const auto distance = m[3][3] - m[3][1];
      _planes.push_back(math::plane{normal, distance}.normalize());
    }

    // Far
    {
      const auto normal = math::vector3{m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2]};
      const auto distance = m[3][3] - m[3][2];
      _planes.push_back(math::plane{normal, distance}.normalize());
    }

    // Near
    {
      const auto normal = math::vector3{m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2]};
      const auto distance = m[3][3] + m[3][2];
      _planes.push_back(math::plane{normal, distance}.normalize());
    }
  }

  auto intersects(const math::matrix4x4& model, const collider& collider) const noexcept -> bool {
    return std::visit([this, &model](const auto& value) { return _intersects(std::decay_t<decltype(value)>::transformed(value, model)); }, collider);
  } 

private:

  auto _intersects(const aabb_collider& aabb) const noexcept -> bool {
    const auto corners = aabb.corners();

    for (const auto& plane : _planes) {
      auto outside_count = 0u;

      for (const auto& corner : corners) {
        if (plane.distance_to_point(corner) < 0.0f) {
          ++outside_count;
        }
      }

      if (outside_count == 8u) {
        return false;
      }
    }

    return true;
  }

  auto _intersects(const sphere_collider& sphere) const noexcept -> bool {
    for (const auto& plane : _planes) {
      if (plane.distance_to_point(sphere.center()) < -sphere.radius()) {
        return false;
      }
    }

    return true;
  }

  template<std::size_t Side>
  auto _extract_plane(const math::matrix4x4& matrix) const -> math::plane {
    constexpr auto negate = (Side == right_plane || Side == top_plane || Side == far_plane);

    const auto column = matrix[3] + (negate ? -matrix[Side] : matrix[Side]);
    const auto normal = math::vector3{column};
    const auto length = normal.length();
  
    return math::plane{normal / length, column.w() / length};
  }

  containers::static_vector<math::plane, 6u> _planes;

}; // struct frustum

class camera {

public:

  camera(const math::angle& field_of_view, std::float_t aspect_ratio, std::float_t near_plane, std::float_t far_plane)
  : _field_of_view{field_of_view},
    _aspect_ratio{aspect_ratio},
    _near_plane{near_plane},
    _far_plane{far_plane} {
    _update_projection();
  }


  auto field_of_view() const noexcept -> const math::angle& {
    return _field_of_view;
  }

  auto set_field_of_view(const math::angle& field_of_view) -> void {
    if (field_of_view == _field_of_view) {
      return;
    }

    _field_of_view = field_of_view;
    _update_projection();
  }

  auto aspect_ratio() const noexcept -> std::float_t {
    return _aspect_ratio;
  }

  auto set_aspect_ratio(std::float_t aspect_ratio) noexcept -> void {
    if (aspect_ratio != _aspect_ratio) {
      _aspect_ratio = aspect_ratio;
      _update_projection();
    }
  }

  auto near_plane() const noexcept -> std::float_t {
    return _near_plane;
  }

  auto far_plane() const noexcept -> std::float_t {
    return _far_plane;
  }

  auto projection() const noexcept -> const math::matrix4x4& {
    return _projection;
  }

  auto view_frustum(const math::matrix4x4& view) const noexcept -> frustum {
    return frustum{_projection * view};
  }

private:

  auto _update_projection() -> void {
    _projection = math::matrix4x4::perspective(_field_of_view, _aspect_ratio, _near_plane, _far_plane);
  }

  math::angle _field_of_view;
  std::float_t _aspect_ratio;
  std::float_t _near_plane;
  std::float_t _far_plane;

  math::matrix4x4 _projection;

}; // class camera

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
