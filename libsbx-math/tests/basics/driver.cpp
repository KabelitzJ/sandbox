#include <iostream>

#include <gtest/gtest.h>

TEST(libsbx_math, base) {
  EXPECT_EQ(true, true);
}

auto main(int argc, char** argv) -> int {
  testing::InitGoogleTest(&argc, argv);

  std::cout << "\nn\n=====================================================================================\n\n";

  return RUN_ALL_TESTS();
}