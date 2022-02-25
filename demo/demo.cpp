#include <iostream>
#include <cstddef>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <container/container.hpp>
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

  auto set = sbx::sparse_set<unsigned int, std::size_t{256}>{};
    
  return EXIT_SUCCESS;
}