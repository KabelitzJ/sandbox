#include <iostream>
#include <cstddef>

#include <math/math.hpp>
#include <types/types.hpp>
#include <ecs/ecs.hpp>

int main() {
  auto v2 = sbx::vector2::zero;

  auto v3 = sbx::vector3::zero;

  auto v4 = sbx::vector4::zero;

  auto m2 = sbx::matrix4x4::identity;

  auto r = sbx::registry{};

  auto e = r.create_entity();

  r.destroy_entity(e);

  std::cout << "Hello, World!" << std::endl;

  return EXIT_SUCCESS;
}