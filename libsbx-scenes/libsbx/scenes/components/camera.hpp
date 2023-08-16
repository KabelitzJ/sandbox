#ifndef LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_
#define LIBSBX_SCENES_COMPONENTS_CAMERA_HPP_

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::scenes {

class camera {

public:

  camera(const math::angle& field_of_view, std::float_t aspect_ratio, std::float_t near_plane, std::float_t far_plane, bool is_active = true)
  : _field_of_view{field_of_view},
    _aspect_ratio{aspect_ratio},
    _near_plane{near_plane},
    _far_plane{far_plane},
    _is_active{is_active} {
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
    if (aspect_ratio == _aspect_ratio) {
      return;
    }

    _aspect_ratio = aspect_ratio;
    _update_projection();
  }

  auto is_active() const noexcept -> bool {
    return _is_active;
  }

  auto set_is_active(bool is_active) noexcept -> void {
    _is_active = is_active;
  }

  auto make_active() noexcept -> void {
    set_is_active(true);
  }

  auto projection() const noexcept -> const math::matrix4x4& {
    return _projection;
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
