#include <gtest/gtest.h>

#include <utility>

#include <math/vector3.hpp>

TEST(sbx_math_vector3, initial_state_default_constructor) {
  const sbx::vector3 vector{};

  EXPECT_EQ(vector.x, sbx::vector3::value_type{0.0f});
  EXPECT_EQ(vector.y, sbx::vector3::value_type{0.0f});
  EXPECT_EQ(vector.z, sbx::vector3::value_type{0.0f});
}

TEST(sbx_math_vector3, initial_state_single_value_constructor) {
  const sbx::vector3 vector{2.0f};

  EXPECT_EQ(vector.x, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(vector.y, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(vector.z, sbx::vector3::value_type{2.0f});
}

TEST(sbx_math_vector3, initial_state_value_constructor) {
  const sbx::vector3 vector{2.0f, 0.4f, -1.0f};

  EXPECT_EQ(vector.x, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(vector.y, sbx::vector3::value_type{0.4f});
  EXPECT_EQ(vector.z, sbx::vector3::value_type{-1.0f});
}

TEST(sbx_math_vector3, initial_state_copy_constructor) {
  const sbx::vector3 vector{2.0f, 0.4f, -1.0f};
  const sbx::vector3 copy{vector}; 

  EXPECT_EQ(vector.x, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(vector.y, sbx::vector3::value_type{0.4f});
  EXPECT_EQ(vector.z, sbx::vector3::value_type{-1.0f});

  EXPECT_EQ(copy.x, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(copy.y, sbx::vector3::value_type{0.4f});
  EXPECT_EQ(copy.z, sbx::vector3::value_type{-1.0f});
}

TEST(sbx_math_vector3, initial_state_move_constructor) {
  const sbx::vector3 vector{2.0f, 0.4f, -1.0f};
  const sbx::vector3 copy{std::move(vector)}; 

  // [NOTE] KAJ 2022-01-22 14:50 - The state of the moved-from object is not relevant. Primitives are noramlly just copied anyway.

  EXPECT_EQ(copy.x, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(copy.y, sbx::vector3::value_type{0.4f});
  EXPECT_EQ(copy.z, sbx::vector3::value_type{-1.0f});
}

TEST(sbx_math_vector3, initial_state_copy_assignment) {
  const sbx::vector3 vector{2.0f, 0.4f, -1.0f};
  sbx::vector3 copy{};

  copy = vector;

  EXPECT_EQ(vector.x, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(vector.y, sbx::vector3::value_type{0.4f});
  EXPECT_EQ(vector.z, sbx::vector3::value_type{-1.0f});

  EXPECT_EQ(copy.x, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(copy.y, sbx::vector3::value_type{0.4f});
  EXPECT_EQ(copy.z, sbx::vector3::value_type{-1.0f});
}

TEST(sbx_math_vector3, initial_state_move_assignment) {
  const sbx::vector3 vector{2.0f, 0.4f, -1.0f};
  sbx::vector3 copy{};

  copy = std::move(vector);

  // [NOTE] KAJ 2022-01-22 14:50 - The state of the moved-from object is not relevant. Primitives are noramlly just copied anyway.

  EXPECT_EQ(copy.x, sbx::vector3::value_type{2.0f});
  EXPECT_EQ(copy.y, sbx::vector3::value_type{0.4f});
  EXPECT_EQ(copy.z, sbx::vector3::value_type{-1.0f});
}

TEST(sbx_math_vector3, normalize) {
  sbx::vector3 vector{2.0f, 0.4f, -1.0f};
  vector.normalize();

  EXPECT_EQ(vector.length(), sbx::vector3::length_type{1.0f});
}

TEST(sbx_math_vector3, normalize_zero_length_vector) {
  sbx::vector3 vector{0.0f, 0.0f, 0.0f};
  vector.normalize();

  EXPECT_EQ(vector.length(), sbx::vector3::length_type{0.0f});
  EXPECT_EQ(vector.x, sbx::vector3::value_type{0.0f});
  EXPECT_EQ(vector.y, sbx::vector3::value_type{0.0f});
  EXPECT_EQ(vector.z, sbx::vector3::value_type{0.0f});
}


