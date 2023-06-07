#include <iostream>

#include <gtest/gtest.h>

#include <libsbx/scene/scene.hpp>

TEST(libsbx_scene, root) {
  EXPECT_EQ(true, true);
}

auto main(int argc, char** argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
