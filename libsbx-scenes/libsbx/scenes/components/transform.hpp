#ifndef LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
#define LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_

#include <vector>
#include <memory>
#include <algorithm>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/ecs/entity.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/quaternion.hpp>
#include <libsbx/math/matrix4x4.hpp>

namespace sbx::scenes {

class transform {

public:

  transform(memory::observer_ptr<transform> parent = nullptr)
  : transform{math::vector3{0.0f, 0.0f, 0.0f}, math::quaternion{0.0f, 0.0f, 0.0f}, math::vector3{1.0f, 1.0f, 1.0f}, parent} { }

  transform(const math::vector3& position, const math::quaternion& rotation, const math::vector3& scale, memory::observer_ptr<transform> parent = nullptr)
  : _position{position},
    _rotation{rotation},
    _scale{scale},
    _parent{parent} { }

  auto local_position() const -> const math::vector3& {
    return _position;
  }

  auto set_local_position(const math::vector3& position) -> void {
    _position = position;
  }

  auto local_rotation() const -> const math::quaternion& {
    return _rotation;
  }

  auto set_local_rotation(const math::quaternion& rotation) -> void {
    _rotation = rotation;
  }

  auto local_scale() const -> const math::vector3& {
    return _scale;
  }

  auto set_local_scale(const math::vector3& scale) -> void {
    _scale = scale;
  }

  auto parent() const -> memory::observer_ptr<transform> {
    return _parent;
  }

  auto add_child(memory::observer_ptr<transform> child) -> void {
    if (!child) {
      return;
    }

    child->_parent = this;
    _children.push_back(child);
  }

  auto matrix() const -> math::matrix4x4 {
    const auto translation = math::matrix4x4::translated(math::matrix4x4::identity, _position);
    const auto rotation = _rotation.to_matrix();
    const auto scale = math::matrix4x4::scaled(math::matrix4x4::identity, _scale);

    const auto local_transform = translation * rotation * scale;

    if (_parent) {
      return _parent->matrix() * local_transform;
    } 

    return local_transform;
  }

private:

  math::vector3 _position;
  math::quaternion _rotation;
  math::vector3 _scale;

  memory::observer_ptr<transform> _parent;
  std::vector<memory::observer_ptr<transform>> _children;

}; // class transform

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
