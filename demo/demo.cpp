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

  auto q = sbx::quaternion{};

  auto set = sbx::sparse_set<sbx::uint32>{};

  auto container = sbx::component_container<sbx::uint32, foo>{};

  auto r = sbx::registry{};

  r.add_component<foo>(0, sbx::uint32{32});
  r.add_component<foo>(1, sbx::uint32{32});
  r.add_component<foo>(2, sbx::uint32{32});
  r.add_component<foo>(3, sbx::uint32{32});
  r.add_component<foo>(4, sbx::uint32{32});
    
  return EXIT_SUCCESS;
}