#include <libsbx/physics/collider.hpp>

namespace sbx::physics {

static auto support(const math::vector3& direction, const box& box, const math::matrix4x4& model, const math::vector3& position) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(model);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  auto result = math::vector3{};

  result.x() = (local_direction.x() > 0.0f) ? box.max.x() : box.min.x();
  result.y() = (local_direction.y() > 0.0f) ? box.max.y() : box.min.y();
  result.z() = (local_direction.z() > 0.0f) ? box.max.z() : box.min.z();

  return math::vector3{model * math::vector4{result, 1.0f}} + position;
}

static auto support(const math::vector3& direction, const sphere& sphere, const math::matrix4x4& model, const math::vector3& position) -> math::vector3 {
  return math::vector3::normalized(direction) * sphere.radius + position;
}

static auto support(const math::vector3& direction, const cylinder& cylinder, const math::matrix4x4& model, const math::vector3& position) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(model);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  const auto local_direction_xz = math::vector3{local_direction.x(), 0.0f, local_direction.z()};

  auto result = math::vector3::normalized(local_direction_xz) * cylinder.radius;
  result.y() = (local_direction.y() > 0.0f) ? cylinder.cap : cylinder.base;

  return math::vector3{model * math::vector4{result, 1.0f}} + position;
}

static auto support(const math::vector3& direction, const capsule& capsule, const math::matrix4x4& model, const math::vector3& position) -> math::vector3 {
  const auto inverse = math::matrix4x4::inverted(model);

  const auto local_direction = math::vector3{inverse * math::vector4{direction, 0.0f}};

  auto result = math::vector3::normalized(local_direction) * capsule.radius;
  result.y() = (local_direction.y() > 0.0f) ? capsule.cap : capsule.base;

  return math::vector3{model * math::vector4{result, 1.0f}} + position;
}

auto support(const math::vector3& direction, const collider& collider, const math::matrix4x4& model, const math::vector3& position) -> math::vector3 {
  return std::visit([&](const auto& shape) { return support(direction, shape, model, position); }, collider);
}

} // namespace sbx::physics
