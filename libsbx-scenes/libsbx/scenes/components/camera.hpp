#ifndef LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
#define LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_

#include <variant>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/volume.hpp>
#include <libsbx/math/sphere.hpp>
#include <libsbx/math/plane.hpp>
#include <libsbx/math/box.hpp>

#include <libsbx/containers/static_vector.hpp>

namespace sbx::scenes {

using aabb_collider = math::volume;
using sphere_collider = math::sphere;

using collider = std::variant<aabb_collider, sphere_collider>;

auto to_volume(const collider& collider) -> math::volume;

auto to_volume(const aabb_collider& collider) -> math::volume;
auto to_volume(const sphere_collider& collider) -> math::volume;

class frustum : public math::box {

  inline static constexpr auto left_plane = std::size_t{0u};
  inline static constexpr auto right_plane = std::size_t{0u};
  inline static constexpr auto top_plane = std::size_t{1u};
  inline static constexpr auto bottom_plane = std::size_t{1u};
  inline static constexpr auto near_plane = std::size_t{2u};
  inline static constexpr auto far_plane = std::size_t{2u};

public:

  frustum(const math::matrix4x4& view_projection)
  : math::box{_extract_planes(view_projection)} { }

  auto intersects(const math::matrix4x4& model, const collider& collider) const noexcept -> bool {
    return std::visit([this, &model](const auto& value) { return _intersects(std::decay_t<decltype(value)>::transformed(value, model)); }, collider);
  } 

private:

  static auto _extract_planes(const math::matrix4x4& matrix) -> std::array<math::plane, 6u> {
    return std::array<math::plane, 6u>{
      _extract_plane<right_plane>(matrix),
      _extract_plane<left_plane>(matrix),
      _extract_plane<bottom_plane>(matrix),
      _extract_plane<top_plane>(matrix),
      _extract_plane<far_plane>(matrix),
      _extract_plane<near_plane>(matrix)
    };
  }

  template<std::size_t Side>
  static auto _extract_plane(const math::matrix4x4& matrix) -> math::plane {
    constexpr auto sign = (Side == right_plane || Side == top_plane || Side == far_plane) ? -1.0f : 1.0f;

    const auto normal = math::vector3{
      matrix[0][3] + sign * matrix[0][Side], 
      matrix[1][3] + sign * matrix[1][Side], 
      matrix[2][3] + sign * matrix[2][Side]
    };

    const auto distance = matrix[3][3] - matrix[3][Side];

    return math::plane{normal, distance}.normalize();
  }

  auto _intersects(const aabb_collider& aabb) const noexcept -> bool {
    const auto corners = aabb.corners();

    for (const auto& plane : planes()) {
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
    for (const auto& plane : planes()) {
      if (plane.distance_to_point(sphere.center()) < -sphere.radius()) {
        return false;
      }
    }

    return true;
  }

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
