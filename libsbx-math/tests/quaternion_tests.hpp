#ifndef LIBSBX_MATH_TESTS_QUATERNION_TESTS_HPP_
#define LIBSBX_MATH_TESTS_QUATERNION_TESTS_HPP_

#include <gtest/gtest.h>

#include <libsbx/math/quaternion.hpp>

TEST(libsbx_math_quaternion, identity_constructor) {
  const auto quaternion = sbx::math::quaternion::identity;

  EXPECT_EQ(quaternion.axis().x, 0.0f);
  EXPECT_EQ(quaternion.axis().y, 0.0f);
  EXPECT_EQ(quaternion.axis().z, 0.0f);
  EXPECT_EQ(quaternion.angle().to_radians(), 1.0f);
}

TEST(libsbx_math_quaternion, constructor) {
  const auto quaternion = sbx::math::quaternion{sbx::math::vector3{1.0f, 2.0f, 3.0f}, sbx::math::radian{4.0f}};

  EXPECT_EQ(quaternion.axis().x, 1.0f);
  EXPECT_EQ(quaternion.axis().y, 2.0f);
  EXPECT_EQ(quaternion.axis().z, 3.0f);
  EXPECT_EQ(quaternion.angle().to_radians(), 4.0f);
}

TEST(libsbx_math_quaternion, addition_assignment_operator) {
  auto quaternion = sbx::math::quaternion{sbx::math::vector3{1.0f, 2.0f, 3.0f}, sbx::math::radian{4.0f}};
  const auto other = sbx::math::quaternion{sbx::math::vector3{5.0f, 6.0f, 7.0f}, sbx::math::radian{8.0f}};

  quaternion += other;

  EXPECT_EQ(quaternion.axis().x, 6.0f);
  EXPECT_EQ(quaternion.axis().y, 8.0f);
  EXPECT_EQ(quaternion.axis().z, 10.0f);
  EXPECT_EQ(quaternion.angle().to_radians(), 12.0f);
}

TEST(libsbx_math_quaternion, subtraction_assignment_operator) {
  auto quaternion = sbx::math::quaternion{sbx::math::vector3{1.0f, 2.0f, 3.0f}, sbx::math::radian{4.0f}};
  const auto other = sbx::math::quaternion{sbx::math::vector3{5.0f, 6.0f, 7.0f}, sbx::math::radian{8.0f}};

  quaternion -= other;

  EXPECT_EQ(quaternion.axis().x, -4.0f);
  EXPECT_EQ(quaternion.axis().y, -4.0f);
  EXPECT_EQ(quaternion.axis().z, -4.0f);
  EXPECT_EQ(quaternion.angle().to_radians(), -4.0f);
}

#endif // LIBSBX_MATH_TESTS_QUATERNION_TESTS_HPP_
