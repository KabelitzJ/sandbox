#include <gtest/gtest.h>

#include <ecs/storage.hpp>

TEST(Storage, InitialState) {
  using storage = sbx::basic_storage<unsigned int, int>;

  auto instance = storage{};

  EXPECT_EQ(0, instance.size());
  EXPECT_EQ(0, instance.capacity());
  EXPECT_TRUE(instance.is_empty());
}

int main(int, char**) {
  testing::InitGoogleTest();

  return RUN_ALL_TESTS();
}