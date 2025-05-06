#include <gtest/gtest.h>

#include <libsbx/ecs/registry.hpp>

class node {

  friend struct sbx::ecs::entity_traits<node>;

public:

  inline static constexpr auto null = sbx::ecs::null_entity;

  using entity_type = std::uint32_t;

  constexpr operator entity_type() const noexcept {
    return _value;
  }

  constexpr operator bool() const noexcept {
    return _value != null;
  }

private:

  constexpr node(const entity_type value) noexcept 
  : _value{value} { }

  entity_type _value;

}; // struct node

using registry_type = sbx::ecs::basic_registry<node>;

struct hierarchy {
  node parent{node::null};
  node first_child{node::null};
  node next_sibling{node::null};
  node previous_sibling{node::null};
}; // struct hierarchy

TEST(libsbx_math_registry, initialize) {
  auto registry = registry_type{};

  auto e1 = registry.create();
  auto e2 = registry.create();
  auto e3 = registry.create();
  auto e4 = registry.create();

  auto& h1 = registry.emplace<hierarchy>(e1);
  auto& h2 = registry.emplace<hierarchy>(e2);
  auto& h3 = registry.emplace<hierarchy>(e3);
  auto& h4 = registry.emplace<hierarchy>(e4);

  if (h1.parent) {

  }
}

auto main(int argc, char* argv[]) -> int {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
