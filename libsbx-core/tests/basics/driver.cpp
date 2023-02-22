#include <iostream>

#include <gtest/gtest.h>

#include <libsbx/core/core.hpp>

TEST(libsbx_core, base) {
  EXPECT_EQ(true, true);
}

TEST(libsbx_core, delegate) {
  auto simple_delegate = sbx::core::delegate<int(int)>{[](int a) { return a + 1; }};

  ASSERT_EQ(simple_delegate.is_valid(), true);
  EXPECT_EQ(simple_delegate(1), 2);

  auto copy = simple_delegate;

  ASSERT_EQ(copy.is_valid(), true);
  EXPECT_EQ(copy(1), 2);  
  ASSERT_EQ(simple_delegate.is_valid(), true);
  EXPECT_EQ(simple_delegate(1), 2);

  auto move = std::move(simple_delegate);

  ASSERT_EQ(move.is_valid(), true);
  EXPECT_EQ(move(1), 2);
  ASSERT_EQ(simple_delegate.is_valid(), false);
}

auto main(int argc, char** argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
