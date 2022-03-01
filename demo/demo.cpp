#include <iostream>
#include <cstddef>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <ecs/ecs.hpp>

struct foo {
  sbx::uint32 i{};
};

enum class node_id : sbx::uint32 {};

int main() {

  auto v2 = sbx::vector2::zero;
  auto v3 = sbx::vector3::zero;
  auto v4 = sbx::vector4::zero;

  auto m4 = sbx::matrix4x4::zero;

  auto quaterion = sbx::quaternion{};

  auto storage = sbx::component_map<node_id, foo>{};

  storage.emplace(node_id{0});

  std::cout << storage.contains(node_id{0}) << std::endl;

  auto& f = storage.get(node_id{0});

  f.i = 4;

  std::cout << storage.get(node_id{0}).i << std::endl;

  storage.remove(node_id{0});
  storage.remove(node_id{1});

  std::cout << storage.contains(node_id{0}) << std::endl;
  std::cout << storage.contains(node_id{1}) << std::endl;

  return EXIT_SUCCESS;
}