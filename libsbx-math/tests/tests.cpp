#include <gtest/gtest.h>

#include <tests/vector2_tests.hpp>
#include <tests/vector3_tests.hpp>

auto main(int argc, char* argv[]) -> int {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
