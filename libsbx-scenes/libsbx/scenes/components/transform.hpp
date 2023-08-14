#ifndef LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
#define LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::scenes {

class transform final {

public:

  transform(const math::vector3& position = math::vector3::zero, const math::vector3& rotation = math::vector3::zero, const math::vector3& scale = math::vector3::one)
  : _position{position}, 
    _rotation{rotation}, 
    _scale{scale} { }

  ~transform() = default;

  auto position() const noexcept -> const math::vector3& {
    return _position;
  }

  auto set_position(const math::vector3& position) noexcept -> void {
    _position = position;
  }

  auto rotation() const noexcept -> const math::vector3& {
    return _rotation;
  }

  auto set_rotation(const math::vector3& rotation) noexcept -> void {
    _rotation = rotation;
  }

  auto scale() const noexcept -> const math::vector3& {
    return _scale;
  }

  auto set_scale(const math::vector3& scale) noexcept -> void {
    _scale = scale;
  }

  auto as_matrix() const -> math::matrix4x4 {
    auto result = math::matrix4x4::identity;

    result = math::matrix4x4::translated(result, _position);

    result = math::matrix4x4::rotated(result, math::vector3::right, math::degree{_rotation.x});
    result = math::matrix4x4::rotated(result, math::vector3::forward, math::degree{_rotation.y});
    result = math::matrix4x4::rotated(result, math::vector3::up, math::degree{_rotation.z});

    result = math::matrix4x4::scaled(result, _scale);

    return result;
  }

private:

  math::vector3 _position;
  math::vector3 _rotation;
  math::vector3 _scale;

}; // class transform

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
