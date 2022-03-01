#include <iostream>
#include <cstddef>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <ecs/ecs.hpp>

struct foo {
  sbx::uint32 i{};
};

enum class node : sbx::uint32 {};

int main() {

  auto v2 = sbx::vector2::zero;
  auto v3 = sbx::vector3::zero;
  auto v4 = sbx::vector4::zero;

  auto m4 = sbx::matrix4x4::zero;

  auto quaterion = sbx::quaternion{};

  auto r = sbx::registry<node>{};

  const auto e1 = r.create_entity();
  const auto e2 = r.create_entity();
  const auto e3 = r.create_entity();

  return EXIT_SUCCESS;
}