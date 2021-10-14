#include <gtest/gtest.h>


TEST(Storage, InitialState) {
  EXPECT_EQ(0, 0);
}

int main(int, char**) {
  testing::InitGoogleTest();

  return RUN_ALL_TESTS();
}