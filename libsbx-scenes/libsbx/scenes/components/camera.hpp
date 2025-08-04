#ifndef LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
#define LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_

#include <variant>

#include <array>
#include <cstdint>

#include <libsbx/utility/enum.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix3x3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/volume.hpp>
#include <libsbx/math/sphere.hpp>
#include <libsbx/math/plane.hpp>
#include <libsbx/math/box.hpp>

#include <libsbx/containers/static_vector.hpp>

#include <libsbx/core/engine.hpp>

// #include <libsbx/devices/devices_module.hpp>
// #include <libsbx/devices/window.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::scenes {

using aabb_collider = math::volume;
using sphere_collider = math::sphere;

using collider = std::variant<aabb_collider, sphere_collider>;

auto to_volume(const collider& collider) -> math::volume;

auto to_volume(const aabb_collider& collider) -> math::volume;
auto to_volume(const sphere_collider& collider) -> math::volume;

// class frustum {

// public:

//   frustum(const math::matrix4x4& view_projection) {
//     const auto m = math::matrix4x4::transposed(view_projection);

//     _planes[0] = m[3] + m[0];
//     _planes[1] = m[3] - m[0];
//     _planes[2] = m[3] + m[1];
//     _planes[3] = m[3] - m[1];
//     _planes[4] = m[3] + m[2];
//     _planes[5] = m[3] - m[2];

//     for (auto& plane : _planes) {
//       plane.normalize();
//     }
//   }

//   auto intersects(const math::matrix4x4& model, const math::volume& volume) const noexcept -> bool {
//     const auto v = math::volume::transformed(volume, model);

//     for (const auto& plane : _planes) {
//       const auto& n = plane.normal();
//       const auto d = plane.distance();

//       auto vp = math::vector3{
//         (n.x() >= 0 ? volume.max().x() : volume.min().x()),
//         (n.y() >= 0 ? volume.max().y() : volume.min().y()),
//         (n.z() >= 0 ? volume.max().z() : volume.min().z())
//       };

//       if (math::vector3::dot(n, vp) + d < 0.0f) {
//         return false;
//       }
//     }

//     return true;
//   } 

// private:

//   std::array<math::plane, 6u> _planes;

// }; // class frustum

class frustum : public math::box {

  inline static constexpr auto left_plane = std::size_t{0u};
  inline static constexpr auto right_plane = std::size_t{0u};
  inline static constexpr auto top_plane = std::size_t{1u};
  inline static constexpr auto bottom_plane = std::size_t{1u};
  inline static constexpr auto near_plane = std::size_t{2u};
  inline static constexpr auto far_plane = std::size_t{2u};

  inline static constexpr auto margin = std::float_t{0.5f};

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

    const auto plane = math::vector4{
      matrix[0][3] + sign * matrix[0][Side], 
      matrix[1][3] + sign * matrix[1][Side], 
      matrix[2][3] + sign * matrix[2][Side],
      matrix[3][3] + sign * matrix[3][Side]
    };

    return math::plane{plane}.normalize();
  }

  auto _intersects(const aabb_collider& aabb) const noexcept -> bool {
    // Corner test

    // const auto corners = aabb.corners();

    // for (const auto& plane : planes()) {
    //   auto outside_count = 0u;

    //   for (const auto& corner : corners) {
    //     if (plane.distance_to_point(corner) < 0.0f) {
    //       ++outside_count;
    //     }
    //   }

    //   if (outside_count == 8u) {
    //     return false;
    //   }
    // }

    // return true;

    // p-vertex test

    for (const auto& plane : planes()) {
      const auto vp = math::vector3{
        (plane.normal().x() >= 0 ? aabb.max().x() : aabb.min().x()),
        (plane.normal().y() >= 0 ? aabb.max().y() : aabb.min().y()),
        (plane.normal().z() >= 0 ? aabb.max().z() : aabb.min().z())
      };

      if (plane.distance_to_point(vp) < -margin) {
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

// class frustum {

//   inline static constexpr auto left = std::uint8_t{0u};
//   inline static constexpr auto right = std::uint8_t{1u};
//   inline static constexpr auto bottom = std::uint8_t{2u};
//   inline static constexpr auto top = std::uint8_t{3u};
//   inline static constexpr auto near = std::uint8_t{4u};
//   inline static constexpr auto far = std::uint8_t{5u};
//   inline static constexpr auto count = std::uint8_t{6u};
//   inline static constexpr auto combinations = count * (count - 1u) / 2u;

// public:

//   frustum(const math::matrix4x4& view_projection) {
//     const auto transposed = math::matrix4x4::transposed(view_projection);
//     _planes[left]   = transposed[3] + transposed[0];
//     _planes[right]  = transposed[3] - transposed[0];
//     _planes[bottom] = transposed[3] + transposed[1];
//     _planes[top]    = transposed[3] - transposed[1];
//     _planes[near]   = transposed[3] + transposed[2];
//     _planes[far]    = transposed[3] - transposed[2];

//     const auto crosses = std::array<math::vector3, combinations> {
//       math::vector3::cross(math::vector3{_planes[left]}, math::vector3{_planes[right]}),
//       math::vector3::cross(math::vector3{_planes[left]}, math::vector3{_planes[bottom]}),
//       math::vector3::cross(math::vector3{_planes[left]}, math::vector3{_planes[top]}),
//       math::vector3::cross(math::vector3{_planes[left]}, math::vector3{_planes[near]}),
//       math::vector3::cross(math::vector3{_planes[left]}, math::vector3{_planes[far]}),
//       math::vector3::cross(math::vector3{_planes[right]}, math::vector3{_planes[bottom]}),
//       math::vector3::cross(math::vector3{_planes[right]}, math::vector3{_planes[top]}),
//       math::vector3::cross(math::vector3{_planes[right]}, math::vector3{_planes[near]}),
//       math::vector3::cross(math::vector3{_planes[right]}, math::vector3{_planes[far]}),
//       math::vector3::cross(math::vector3{_planes[bottom]}, math::vector3{_planes[top]}),
//       math::vector3::cross(math::vector3{_planes[bottom]}, math::vector3{_planes[near]}),
//       math::vector3::cross(math::vector3{_planes[bottom]}, math::vector3{_planes[far]}),
//       math::vector3::cross(math::vector3{_planes[top]}, math::vector3{_planes[near]}),
//       math::vector3::cross(math::vector3{_planes[top]}, math::vector3{_planes[far]}),
//       math::vector3::cross(math::vector3{_planes[near]}, math::vector3{_planes[far]})
//     };

//     _corners[0] = _intersection<left, bottom, near>(crosses);
//     _corners[1] = _intersection<left, top, near>(crosses);
//     _corners[2] = _intersection<right, bottom, near>(crosses);
//     _corners[3] = _intersection<right, top, near>(crosses);
//     _corners[4] = _intersection<left, bottom, far>(crosses);
//     _corners[5] = _intersection<left, top, far>(crosses);
//     _corners[6] = _intersection<right, bottom, far>(crosses);
//     _corners[7] = _intersection<right, top, far>(crosses);
//   }

//   auto intersects(const math::matrix4x4& model, const math::volume& volume) const noexcept -> bool {
//     const auto transformed = math::volume::transformed(volume, model);

//     const auto& min = transformed.min();
//     const auto& max = transformed.max();

//     // check box outside/inside of frustum
//     for (const auto& plane : _planes) {
//       if (
//         (math::vector3::dot(plane, math::vector4(min.x(), min.y(), min.z(), 1.0f)) < 0.0f) &&
//         (math::vector3::dot(plane, math::vector4(max.x(), min.y(), min.z(), 1.0f)) < 0.0f) &&
//         (math::vector3::dot(plane, math::vector4(min.x(), max.y(), min.z(), 1.0f)) < 0.0f) &&
//         (math::vector3::dot(plane, math::vector4(max.x(), max.y(), min.z(), 1.0f)) < 0.0f) &&
//         (math::vector3::dot(plane, math::vector4(min.x(), min.y(), max.z(), 1.0f)) < 0.0f) &&
//         (math::vector3::dot(plane, math::vector4(max.x(), min.y(), max.z(), 1.0f)) < 0.0f) &&
//         (math::vector3::dot(plane, math::vector4(min.x(), max.y(), max.z(), 1.0f)) < 0.0f) &&
//         (math::vector3::dot(plane, math::vector4(max.x(), max.y(), max.z(), 1.0f)) < 0.0f)
//       ) {
//         return false;
//       }
//     }

//     // check frustum outside/inside box
//     auto out = 0u;

//     out = 0; for (int i = 0; i<8; i++) out += ((_corners[i].x() > max.x()) ? 1 : 0); if (out == 8) return false;
//     out = 0; for (int i = 0; i<8; i++) out += ((_corners[i].x() < min.x()) ? 1 : 0); if (out == 8) return false;
//     out = 0; for (int i = 0; i<8; i++) out += ((_corners[i].y() > max.y()) ? 1 : 0); if (out == 8) return false;
//     out = 0; for (int i = 0; i<8; i++) out += ((_corners[i].y() < min.y()) ? 1 : 0); if (out == 8) return false;
//     out = 0; for (int i = 0; i<8; i++) out += ((_corners[i].z() > max.z()) ? 1 : 0); if (out == 8) return false;
//     out = 0; for (int i = 0; i<8; i++) out += ((_corners[i].z() < min.z()) ? 1 : 0); if (out == 8) return false;

//     // if (
//     //   _check_corners<true>(max.x()) || 
//     //   _check_corners<false>(min.x()) ||
//     //   _check_corners<true>(max.y()) || 
//     //   _check_corners<false>(min.y()) ||
//     //   _check_corners<true>(max.z()) || 
//     //   _check_corners<false>(min.z())
//     // ) {
//     //   return false;
//     // }

//     return true;
//   }

// private:

//   template<std::uint8_t I, std::uint8_t J>
//   static constexpr auto ij2k() noexcept -> std::uint8_t {
//     return I * (9 - I) / 2 + J - 1;
//   }

//   template<std::uint8_t A, std::uint8_t B, std::uint8_t C>
//   auto _intersection(const std::array<math::vector3, combinations>& crosses) noexcept -> math::vector3 {
//     const auto D = math::vector3::dot(math::vector3{_planes[A]}, crosses[ij2k<B, C>()]);

//     const auto& cross1 = crosses[ij2k<B, C>()];
//     const auto& cross2 = crosses[ij2k<A, C>()];
//     const auto& cross3 = crosses[ij2k<A, B>()];

//     const auto m = math::matrix3x3{cross1, cross2, cross3};

//     const auto v = math::vector3{_planes[A].w(), _planes[B].w(), _planes[C].w()};

// 	  return (m * v) * (-1.0f / D);
//   }

//   template<bool GreaterThan>
//   auto _check_corners(const std::float_t value) const noexcept -> bool {
//     auto out = 0u;

//     for (const auto& corner : _corners) {
//       if constexpr (GreaterThan) {
//         out += (corner.x() > value) ? 1u : 0u;
//       } else {
//         out += (corner.x() < value) ? 1u : 0u;
//       }
//     }

//     return (out == 8u);
//   }

//   std::array<math::vector4, count> _planes;
//   std::array<math::vector3, 8u> _corners;

// }; // class frustum

class camera {

public:

  camera(const math::angle& field_of_view, std::float_t aspect_ratio, std::float_t near_plane, std::float_t far_plane)
  : _field_of_view{field_of_view},
    _aspect_ratio{aspect_ratio},
    _near_plane{near_plane},
    _far_plane{far_plane} {
    _update_projection();

    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    graphics_module.on_viewport_changed() += [this](const math::vector2u& event) {
      set_aspect_ratio(static_cast<std::float_t>(event.x()) / static_cast<std::float_t>(event.y()));
    };
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
