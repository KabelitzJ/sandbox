#ifndef LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
#define LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_

#include <variant>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::scenes {

struct sphere_collider {
  math::vector3 center;
  std::float_t radius;
}; // struct sphere_collider

struct aabb_collider {
  math::vector3 min;
  math::vector3 max;
}; // struct aabb_collider

using collider = std::variant<sphere_collider, aabb_collider>;

class frustum {

public:

  frustum(const math::matrix4x4& view_projection) {
    _planes[0] = _extract_plane(view_projection, 0, 0);
    _planes[1] = _extract_plane(view_projection, 0, 1);
    _planes[2] = _extract_plane(view_projection, 1, 0);
    _planes[3] = _extract_plane(view_projection, 1, 1);
    _planes[4] = _extract_plane(view_projection, 2, 0);
    _planes[5] = _extract_plane(view_projection, 2, 1);
  }

  auto intersects(const collider& collider) const noexcept -> bool {
    return std::visit([this](const auto& collider){ return _intersects(collider); }, collider);
  }

private:

  auto _intersects(const sphere_collider& sphere) const noexcept -> bool {
    for (const auto& plane : _planes) {
      const auto distance = math::vector3::dot(plane.normal, sphere.center) + plane.distance;

      if (distance < -sphere.radius) {
        return false;
      }
    }

    return true;
  }

  auto _intersects(const aabb_collider& aabb) const noexcept -> bool {
    return false;
  }

  struct plane {
    math::vector3 normal;
    std::float_t distance;

    auto normalize() -> void {
      const auto length = normal.length();

      normal /= length;
      distance /= length;
    }
  }; // struct plane

  auto _extract_plane(const math::matrix4x4& matrix, std::size_t row_a, std::size_t row_b) const -> plane {
    const auto row = matrix[3] + matrix[row_a] * ((row_b & 1) ? -1.f : 1.f);
    const auto normal = math::vector3{row};
    const auto length = normal.length();
  
    return plane{normal / length, row.w() / length};
  }

  std::array<plane, 6u> _planes;

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
    const auto view_projection = _projection * view;

    return frustum{view_projection};
  }

private:

  auto _update_projection() -> void {
    _projection = math::matrix4x4::perspective(_field_of_view, _aspect_ratio, _near_plane, _far_plane);
  }

  math::angle _field_of_view;
  std::float_t _aspect_ratio;
  std::float_t _near_plane;
  std::float_t _far_plane;

  bool _is_active;

  math::matrix4x4 _projection;

}; // class camera

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
