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

  std::cout << "Hello, Sandbox!" << std::endl;

  auto v2 = sbx::vector2::zero;
  auto v3 = sbx::vector3::zero;
  auto v4 = sbx::vector4::zero;
  auto m2 = sbx::matrix4x4::identity;

  auto r = sbx::registry{};

  for (auto i = 0; i < 10; ++i) {
    auto e = r.create_entity();
    r.add_component<tag>(e, "tag" + std::to_string(i));
  }

  for (const auto& [e, t] : r.create_view<const tag>()) {
    std::cout << t.tag << ", ";
  }
  
  return EXIT_SUCCESS;
}