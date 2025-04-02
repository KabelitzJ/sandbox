#ifndef LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
#define LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::scenes {

struct sphere_collider {
  math::vector3 center;
  std::float_t radius;
}; // struct sphere_collider

class camera {

public:

  struct frustum_plane {
    math::vector3 normal;
    std::float_t distance;
  }; // struct frustum_plane

  struct frustum {

    std::array<frustum_plane, 6> planes;

    auto contains(const sphere_collider& collider) const noexcept -> bool {
      for (const auto& plane : planes) {
        const auto dist = math::vector3::dot(plane.normal, collider.center) + plane.distance;

        if (dist < -collider.radius) {
          return false;
        }
      }

      return true;
    }

  }; // struct frustum

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

    const auto left = _extract_plane(view_projection, 0, 0);
    const auto right = _extract_plane(view_projection, 0, 1);
    const auto bottom = _extract_plane(view_projection, 1, 0);
    const auto top = _extract_plane(view_projection, 1, 1);
    const auto near = _extract_plane(view_projection, 2, 0);
    const auto far = _extract_plane(view_projection, 2, 1);

    return frustum{left, right, bottom, top, near, far};
  }

private:

  auto _update_projection() -> void {
    _projection = math::matrix4x4::perspective(_field_of_view, _aspect_ratio, _near_plane, _far_plane);
  }

  auto _extract_plane(const math::matrix4x4& matrix, std::size_t row_a, std::size_t row_b) const -> frustum_plane {
    const auto row = matrix[3] + matrix[row_a] * ((row_b & 1) ? -1.f : 1.f);
    const auto normal = math::vector3{row};
    const auto length = normal.length();
  
    return frustum_plane{normal / length, row.w() / length};
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
