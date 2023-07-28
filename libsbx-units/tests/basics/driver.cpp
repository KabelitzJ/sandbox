#include <gtest/gtest.h>

#include <libsbx/units/units.hpp>

TEST(units, version) {
  EXPECT_EQ(true , true);
}

TEST(units, distance_kilometer) {
  using namespace sbx::units::literals;

  auto km = 1.0_km;
  auto m = 1000_m;

  EXPECT_EQ(km.value(), sbx::units::quantity_cast<sbx::units::kilometer>(m).value());
}

auto main(int argc, char** argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
