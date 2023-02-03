#include <iostream>
#include <regex>

#include <gtest/gtest.h>

#include <libsbx/scripting/scripting.hpp>

TEST(libsbx_scripting, root) {
  EXPECT_EQ(true, true);
}

auto main(int argc, char** argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
