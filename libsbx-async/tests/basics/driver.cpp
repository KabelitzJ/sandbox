#include <iostream>

#include <gtest/gtest.h>

#include <libsbx/async/async.hpp>

TEST(sbx_async, base) {
  EXPECT_EQ(true, true);
}

auto main(int argc, char** argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
