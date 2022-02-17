#include <iostream>

#include <utils/type_id.hpp>

#include <math/math.hpp>

#include <ecs/ecs.hpp>

#include <ecs/entity_traits.hpp>

int main() {

  auto vector = sbx::vector3{1.0f, 2.0f, 3.0f};
  auto matrix = sbx::matrix4x4::identity;

  auto result = matrix * sbx::vector4{vector};

  std::cout << result << std::endl;

  using namespace sbx::literals;

  const auto q = sbx::quaternion{sbx::vector3::up, 90.0_degrees};

  // auto r = sbx::registry{};

  auto id = sbx::type_id<sbx::vector3>{};

  std::cout << "vector3 id: " << sbx::uint32{id} << std::endl;

  auto set = sbx::sparse_set{};

  return 0;
}