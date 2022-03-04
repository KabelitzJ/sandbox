#include <iostream>
#include <cstddef>
#include <bitset>

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

  auto r = sbx::registry{};

  auto e = r.create_entity();

  auto& f = r.add_component<foo>(e, sbx::uint32{1});

  std::cout << r.get_component<foo>(e).i << std::endl;

  f.i = 42;

  std::cout << r.get_component<foo>(e).i << std::endl;

  r.create_view();

  return EXIT_SUCCESS;
}