#include <iostream>

#include <gtest/gtest.h>

#include <libsbx/signal/signal.hpp>

auto main(int argc, char** argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
