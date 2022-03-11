#include <iostream>
#include <cstddef>
#include <bitset>

#include <math/math.hpp>
#include <types/types.hpp>
#include <container/container.hpp>
#include <ecs/ecs.hpp>

struct transform {
  sbx::vector3 position{};
  sbx::vector3 scale{};
  sbx::quaternion rotation{};
};

int main() {

  std::cout << "Hello, World!" << std::endl;

  auto v2 = sbx::vector2::zero;
  auto v3 = sbx::vector3::zero;
  auto v4 = sbx::vector4::zero;
  auto m2 = sbx::matrix4x4::identity;

  auto r = sbx::registry{};

  auto e = r.create_entity();
  r.add_component<transform>(e);


  r.destroy_entity(e);

  return EXIT_SUCCESS;
}