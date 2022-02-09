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

  const auto matrix3 = sbx::matrix4x4{
    5.0f, 8.0f, 4.0f, 2.0f,
    9.0f, 0.0f, 4.0f, 0.0f,
    2.0f, 5.0f, 5.0f, 1.0f,
    3.0f, 3.0f, 1.0f, 7.0f
  };

  const auto matrix4 = sbx::matrix4x4{
    6.0f, 3.0f, 4.0f, 7.0f,
    6.0f, 9.0f, 8.0f, 1.0f,
    2.0f, 0.0f, 2.0f, 2.0f,
    9.0f, 2.0f, 4.0f, 3.0f
  };

  const auto expexted_result1 = sbx::matrix4x4{
    7.0f, 8.0f, 0.0f, 0.0f,
    9.0f, 2.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
  };

  const auto expexted_result2 = sbx::matrix4x4{
    104.0f, 91.0f, 100.0f, 57.0f,
    62.0f,  27.0f, 44.0f,  71.0f,
    61.0f,  53.0f, 62.0f,  32.0f,
    101.0f, 50.0f, 66.0f,  47.0f
  };

  const auto result1 = matrix1 * matrix2;

  const auto result2 = matrix3 * matrix4;

  EXPECT_EQ(result1, expexted_result1);
  EXPECT_EQ(result2, expexted_result2);
}

TEST(sbx_math_matrix_4x4, matrix_vector_multiplication) {
  const auto matrix = sbx::matrix4x4{
    1.0f, 0.0f, 3.0f, 0.0f,
    0.0f, 0.0f, 2.0f, 3.0f,
    4.0f, 1.0f, 0.0f, 2.0f,
    0.0f, 3.0f, 4.0f, 0.0f
  };

  const auto vector = sbx::vector4{1.0f, 2.0f, 3.0f, 4.0f};

  const auto expexted_result = sbx::vector4{10.0f, 18.0f, 14.0f, 18.0f};

  const auto result = matrix * vector;

  EXPECT_EQ(result, expexted_result);
}
