#include <cinttypes>

#include <gtest/gtest.h>

#include <libsbx/memory/memory.hpp>

TEST(memory, basics) {
  EXPECT_EQ(true, true);
}

auto main(int argc, char* argv[]) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
