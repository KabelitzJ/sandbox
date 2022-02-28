#include <iostream>
#include <cstddef>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <ecs/ecs.hpp>

struct foo {
  sbx::uint32 i{};
};

int main() {

  auto v2 = sbx::vector2::zero;
  auto v3 = sbx::vector3::zero;
  auto v4 = sbx::vector4::zero;

  auto m4 = sbx::matrix4x4::zero;

  auto quaterion = sbx::quaternion{};

  auto storage = sbx::component_storage<sbx::entity_t, foo>{};

  storage.emplace(sbx::entity_t{0});

  std::cout << storage.contains(sbx::entity_t{0}) << std::endl;

  auto& f = storage.get(sbx::entity_t{0});

  f.i = 4;

  std::cout << storage.get(sbx::entity_t{0}).i << std::endl;
    
  return EXIT_SUCCESS;
}