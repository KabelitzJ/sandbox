#include <gtest/gtest.h>

#include <libsbx/ecs/registry.hpp>

TEST(libsbx_math_registry, initialize) {
  auto registry = sbx::ecs::registry{};

  auto entity = registry.create();

  registry.emplace<std::uint32_t>(entity, 42);
  registry.emplace<std::uint64_t>(entity, 84);

  auto view = registry.view<std::uint32_t, std::uint64_t>();
  // auto view = registry.view<std::uint32_t>();

  for (auto entity : view) {
    // EXPECT_EQ(value1, 42);
    // EXPECT_EQ(value2, 84);
    const auto& value1 = view.get<std::uint32_t>(entity);

    EXPECT_EQ(value1, 42);
  }
}

auto main(int argc, char* argv[]) -> int {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
