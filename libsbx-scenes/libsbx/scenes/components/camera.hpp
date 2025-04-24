#ifndef LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
#define LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_

#include <variant>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::scenes {

struct aabb_collider {
  math::vector3 min;
  math::vector3 max;

  auto center() const noexcept -> math::vector3 {
    return (min + max) * 0.5f;
  }

  auto corners() const noexcept -> std::array<math::vector3, 8u> {
    return std::array<math::vector3, 8u>{
      math::vector3{min.x(), min.y(), min.z()},
      math::vector3{min.x(), min.y(), max.z()},
      math::vector3{min.x(), max.y(), min.z()},
      math::vector3{min.x(), max.y(), max.z()},
      math::vector3{max.x(), min.y(), min.z()},
      math::vector3{max.x(), min.y(), max.z()},
      math::vector3{max.x(), max.y(), min.z()},
      math::vector3{max.x(), max.y(), max.z()}
    };
  }

  static auto transformed(const aabb_collider& aabb, const math::matrix4x4& model) -> aabb_collider {
    auto result = aabb_collider{};

    result.min = math::vector3{std::numeric_limits<std::float_t>::max()};
    result.max = math::vector3{std::numeric_limits<std::float_t>::lowest()};

    for (const auto& corner : aabb.corners()) {
      const auto transformed = math::vector3{model * math::vector4{corner, 1.0f}};
      result.min = math::vector3::min(result.min, transformed);
      result.max = math::vector3::max(result.max, transformed);
    }

    return result;
  }

}; // struct box_collider

struct sphere_collider {
  math::vector3 center;
  std::float_t radius;
}; // struct sphere_collider

using collider = std::variant<aabb_collider, sphere_collider>;

class frustum {

public:

  frustum(const math::matrix4x4& view_projection) {
    const auto& m = view_projection;

    // Right
    _planes[0].normal = math::vector3{m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0]};
    _planes[0].distance = m[3][3] - m[3][0];
    _planes[0].normalize();

    // Left
    _planes[1].normal = math::vector3{m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0]};
    _planes[1].distance = m[3][3] + m[3][0];
    _planes[1].normalize();

    // Bottom
    _planes[2].normal = math::vector3{m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1]};
    _planes[2].distance = m[3][3] + m[3][1];
    _planes[2].normalize();

    // Top
    _planes[3].normal = math::vector3{m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1]};
    _planes[3].distance = m[3][3] - m[3][1];
    _planes[3].normalize();

    // Far
    _planes[4].normal = math::vector3{m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2]};
    _planes[4].distance = m[3][3] - m[3][2];
    _planes[4].normalize();

    // Near
    _planes[5].normal = math::vector3{m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2]};
    _planes[5].distance = m[3][3] + m[3][2];
    _planes[5].normalize();


    // _planes[0] = _extract_plane<plane::left>(view_projection);
    // _planes[1] = _extract_plane<plane::right>(view_projection);
    // _planes[2] = _extract_plane<plane::top>(view_projection);
    // _planes[3] = _extract_plane<plane::bottom>(view_projection);
    // _planes[4] = _extract_plane<plane::near>(view_projection);
    // _planes[5] = _extract_plane<plane::far>(view_projection);
  }

  auto intersects(const math::matrix4x4& model, const collider& collider) const noexcept -> bool {
    return std::visit([this, &model](const auto& value) { return _intersects(model, value); }, collider);
  } 

private:

  auto _intersects(const math::matrix4x4& model, const aabb_collider& aabb) const noexcept -> bool {
    const auto transformed = aabb_collider::transformed(aabb, model);

    const auto corners = transformed.corners();

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

  auto _intersects(const math::matrix4x4& model, const sphere_collider& sphere) const noexcept -> bool {
    const auto center = math::vector3{model * math::vector4{sphere.center, 1.0f}};

    for (const auto& plane : _planes) {
      if (plane.distance_to_point(center) < -sphere.radius) {
        return false;
      }
    }

    return true;
  }

  struct plane {

    inline static constexpr auto left = std::size_t{0u};
    inline static constexpr auto right = std::size_t{0u};
    inline static constexpr auto top = std::size_t{1u};
    inline static constexpr auto bottom = std::size_t{1u};
    inline static constexpr auto near = std::size_t{2u};
    inline static constexpr auto far = std::size_t{2u};

    math::vector3 normal;
    std::float_t distance;

    auto distance_to_point(const math::vector3& point) const noexcept -> std::float_t {
      return math::vector3::dot(normal, point) + distance;
    } 

    auto signed_distance(const math::vector3& point) const noexcept -> std::float_t {
      return math::vector3::dot(normal, point) - distance;
    }

    auto normalize() -> void {
      const auto length = normal.length();
      normal /= length;
      distance /= length;
    }

  }; // struct plane

  template<std::size_t Side>
  auto _extract_plane(const math::matrix4x4& matrix) const -> plane {
    constexpr auto negate = (Side == plane::right || Side == plane::top || Side == plane::far);

    const auto column = matrix[3] + (negate ? -matrix[Side] : matrix[Side]);
    const auto normal = math::vector3{column};
    const auto length = normal.length();
  
    return plane{normal / length, column.w() / length};
  }

  auto _make_plane(const math::vector3& normal, const math::vector3& point_on_plane) -> plane {
    const auto normalized = math::vector3::normalized(normal);
    float distance = -math::vector3::dot(normalized, point_on_plane);
    return plane{normalized, distance};
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

  bool _is_active;

  math::matrix4x4 _projection;

}; // class camera

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
