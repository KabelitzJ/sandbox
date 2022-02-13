#include <iostream>

#include <math/math.hpp>

#include <ecs/ecs.hpp>

int main() {

  auto vector = sbx::vector3{1.0f, 2.0f, 3.0f};
  auto matrix = sbx::matrix4x4::identity;

  auto result = matrix * sbx::vector4{vector};

  std::cout << result << std::endl;

  using namespace sbx::literals;

  const auto q = sbx::quaternion{sbx::vector3::up, 90.0_degrees};

  auto r = sbx::registry{};

  return 0;
}