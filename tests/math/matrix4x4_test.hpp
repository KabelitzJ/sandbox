#include <gtest/gtest.h>

#include <math/vector4.hpp>
#include <math/matrix4x4.hpp>

TEST(sbx_math_matrix_4x4, initial_value) {
  const auto matrix = sbx::matrix4x4{};

  for (auto i = sbx::matrix4x4::index_type{0}; i < 4; ++i) {
    for (auto j = sbx::matrix4x4::index_type{0}; j < 4; ++j) {
      EXPECT_EQ(matrix[i][j], sbx::matrix4x4::value_type{0.0f});
    }
  }
}

TEST(sbx_math_matrix_4x4, initial_value_identity) {
  const auto matrix = sbx::matrix4x4::identity;

  for (auto i = sbx::matrix4x4::index_type{0}; i < 4; ++i) {
    for (auto j = sbx::matrix4x4::index_type{0}; j < 4; ++j) {
      if (i == j) {
        EXPECT_EQ(matrix[i][j], sbx::matrix4x4::value_type{1.0f});
      } else {
        EXPECT_EQ(matrix[i][j], sbx::matrix4x4::value_type{0.0f});
      }
    }
  }
}

TEST(sbx_math_matrix_4x4, matrix_matrix_multiplication) {
  const auto matrix1 = sbx::matrix4x4{
    3.0f, 2.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 2.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
  };

  const auto matrix2 = sbx::matrix4x4{
    1.0f, 2.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    4.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
  };

  const auto expexted_result = sbx::matrix4x4{
    7.0f, 8.0f, 0.0f, 0.0f,
    9.0f, 2.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
  };

  const auto result = matrix1 * matrix2;

  EXPECT_EQ(result, expexted_result);
}

TEST(sbx_math_matrix_4x4, matrix_vector_multiplication) {
  const auto matrix = sbx::matrix4x4{
    3.0f, 2.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 2.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
  };

  const auto vector = sbx::vector4{1.0f, 0.0f, 4.0f, 0.0f};

  const auto expexted_result = sbx::vector4{7.0f, 9.0f, 0.0f, 0.0f};

  const auto result = matrix * vector;

  EXPECT_EQ(result, expexted_result);
}
