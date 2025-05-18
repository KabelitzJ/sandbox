#include <libsbx/scenes/components/camera.hpp>

namespace sbx::scenes {

auto to_volume(const collider& collider) -> math::volume {
  return std::visit([](const auto& value) { return to_volume(value); }, collider);
}

auto to_volume(const aabb_collider& collider) -> math::volume {
  return collider;
}

auto to_volume(const sphere_collider& collider) -> math::volume {
  const auto min = collider.center() - math::vector3{collider.radius()};
  const auto max = collider.center() + math::vector3{collider.radius()};

  return math::volume{min, max};
}

} // namespace sbx::scenes
