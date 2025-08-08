#ifndef LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
#define LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_

#include <numbers>

#include <libsbx/ecs/meta.hpp>

#include <libsbx/math/fwd.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>
#include <libsbx/math/quaternion.hpp>

namespace sbx::math {

class transform final {

  friend struct detail::matrix_cast_impl<4, 4, math::transform>;

public:

  transform(const vector3& position = vector3::zero, const quaternion& rotation = quaternion::identity, const vector3& scale = vector3::one);

  ~transform() = default;

  auto position() const noexcept -> const vector3&;

  auto position() noexcept -> vector3&;

  auto set_position(const vector3& position) noexcept -> void;

  auto move_by(const vector3& offset) noexcept -> void;

  auto rotation() const noexcept -> const quaternion&;

  auto set_rotation(const quaternion& rotation) noexcept -> void;

  auto set_rotation(const vector3& axis, const angle& angle) noexcept -> void;

  auto scale() noexcept -> vector3&;

  auto scale() const noexcept -> const vector3&;

  auto set_scale(const vector3& scale) noexcept -> void;

  auto forward() const noexcept -> vector3;

  auto right() const noexcept -> vector3;

  auto up() const noexcept -> vector3;

  auto look_at(const vector3& target) noexcept -> void;

  auto is_dirty() const noexcept -> bool;

  auto clear_is_dirty() noexcept -> void;

private:

  vector3 _position;
  quaternion _rotation;
  vector3 _scale;

  matrix4x4 _rotation_matrix;

  bool _is_dirty;

}; // class transform

} // namespace sbx::math

template<>
struct sbx::ecs::meta<sbx::math::transform> {
  auto operator()(const utility::hashed_string& tag, sbx::math::transform& value) -> void {
    if (tag == "save") {
      
    }
  }
}; // sbx::ecs::meta

#endif // LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
