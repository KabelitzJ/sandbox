#include <iostream>
#include <cstddef>
#include <type_traits>
#include <bitset>

#include <math/math.hpp>
#include <types/types.hpp>
#include <container/container.hpp>
#include <memory/memory.hpp>
#include <ecs/ecs.hpp>

struct transform {
  sbx::vector3 position{};
  sbx::vector3 scale{};
  sbx::quaternion rotation{};
};

struct foo {
  sbx::uint32 f{};
};

struct bar {
  sbx::uint32 b{};
};

struct tag {
  std::string tag{};
};

int main() {

  std::cout << "Hello, World!" << std::endl;

  auto v2 = sbx::vector2::zero;
  auto v3 = sbx::vector3::zero;
  auto v4 = sbx::vector4::zero;
  auto m2 = sbx::matrix4x4::identity;

  auto r = sbx::registry{};

  auto e1 = r.create_entity();
  auto e2 = r.create_entity();

  r.add_component<foo>(e1);
  r.add_component<bar>(e1, 31u);
  r.add_component<tag>(e1, "e1");

  r.add_component<foo>(e2, 32u);
  r.add_component<bar>(e2, 33u);
  r.add_component<tag>(e2, "e2");

  for (auto& [entity, foo, tag] : r.create_view<foo, const tag>()) {
    std::cout << "(foo) entity: " << tag.tag << " " << foo.f << std::endl;
  }

  for (auto& [entity, bar, tag] : r.create_view<bar, const tag>()) {
    std::cout << "(bar) entity: " << tag.tag << " " << bar.b << std::endl;
  }
  
  return EXIT_SUCCESS;
}