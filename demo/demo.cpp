#include <iostream>
#include <cstddef>
#include <bitset>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <ecs/ecs.hpp>

#include "type_less_container_base.hpp"
#include "container.hpp"

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

  r.destroy_entity(e);

  auto c = demo::container<foo>{};

  for (auto i = 0; i < 10; ++i) {
    c.emplace_back(i);
  }

  c.clear();

  std::cout << "size: " << c.size() << std::endl;

  for (auto& f : c) {
    std::cout << f.i << std::endl;
  }

  for (auto i = 9; i >= 0; --i) {
    c.emplace_back(i);
  }

  std::cout << "size: " << c.size() << std::endl;

  for (auto& f : c) {
    std::cout << f.i << ", ";
  }

  std::cout << std::endl;

  c.remove(0);
  c.remove(2);
  c.remove(4);

  for (auto& f : c) {
    std::cout << f.i << ", ";
  }

  std::cout << std::endl;

  return EXIT_SUCCESS;
}