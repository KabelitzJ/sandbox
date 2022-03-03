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

  auto r = sbx::registry{};

  auto e1 = r.create_entity();
  auto e2 = r.create_entity();
  auto e3 = r.create_entity();

  std::cout << std::boolalpha << r.is_valid_entity(e1) << std::noboolalpha << std::endl;
  std::cout << std::boolalpha << r.is_valid_entity(e2) << std::noboolalpha << std::endl;
  std::cout << std::boolalpha << r.is_valid_entity(e3) << std::noboolalpha << std::endl;

  r.destroy_entity(e1);

  std::cout << std::boolalpha << r.is_valid_entity(e1) << std::noboolalpha << std::endl;

  e1 = r.create_entity();

  std::cout << std::boolalpha << r.is_valid_entity(e1) << std::noboolalpha << std::endl;

  return EXIT_SUCCESS;
}