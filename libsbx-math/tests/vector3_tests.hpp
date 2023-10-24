#ifndef LIBSBX_MATH_VECTOR3_TESTS_HPP_
#define LIBSBX_MATH_VECTOR3_TESTS_HPP_

#include <gtest/gtest.h>

#include <libsbx/math/vector3.hpp>

TEST(libsbx_math_vector3, default_constructor) {
  const auto vector = sbx::math::vector3{};

  EXPECT_EQ(vector.x, 0.0f);
  EXPECT_EQ(vector.y, 0.0f);
  EXPECT_EQ(vector.z, 0.0f);
}

TEST(libsbx_math_vector3, constructor) {
  const auto vector = sbx::math::vector3{1.0f, 2.0f, 3.0f};

  EXPECT_EQ(vector.x, 1.0f);
  EXPECT_EQ(vector.y, 2.0f);
  EXPECT_EQ(vector.z, 3.0f);
}

TEST(libsbx_math_vector3, copy_constructor) {
  const auto vector = sbx::math::vector3{1.0f, 2.0f, 3.0f};
  const auto copy = sbx::math::vector3{vector};

  EXPECT_EQ(copy.x, 1.0f);
  EXPECT_EQ(copy.y, 2.0f);
  EXPECT_EQ(copy.z, 3.0f);
}

TEST(libsbx_math_vector3, move_constructor) {
  auto vector = sbx::math::vector3{1.0f, 2.0f, 3.0f};
  const auto copy = sbx::math::vector3{std::move(vector)};

  EXPECT_EQ(copy.x, 1.0f);
  EXPECT_EQ(copy.y, 2.0f);
  EXPECT_EQ(copy.z, 3.0f);
}

TEST(libsbx_math_vector3, conversion_constructor) {
  const auto vector = sbx::math::vector3{std::int32_t{1}, std::int32_t{2}, std::int32_t{3}};

  EXPECT_EQ(vector.x, 1.0f);
  EXPECT_EQ(vector.y, 2.0f);
  EXPECT_EQ(vector.z, 3.0f);
}

TEST(libsbx_math_vector3, from_vector2_constructor) {
  const auto vector = sbx::math::vector3{sbx::math::vector2{1.0f, 2.0f}, 3.0f};

  EXPECT_EQ(vector.x, 1.0f);
  EXPECT_EQ(vector.y, 2.0f);
  EXPECT_EQ(vector.z, 3.0f);
}

#endif // LIBSBX_MATH_VECTOR3_TESTS_HPP_
