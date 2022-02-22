#include <cassert>
#include <algorithm>
#include <iomanip>

namespace sbx {

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4() noexcept
: _columns{column_type{0}, column_type{0}, column_type{0}, column_type{0}} { }

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(const std::array<column_type, 4>& columns) noexcept
: _columns{columns} { }

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(
  const column_type& column0,
  const column_type& column1,
  const column_type& column2,
  const column_type& column3
) noexcept
: _columns{column0, column1, column2, column3} { }

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(
  const value_type x0, const value_type y0, const value_type z0, const value_type w0,
  const value_type x1, const value_type y1, const value_type z1, const value_type w1,
  const value_type x2, const value_type y2, const value_type z2, const value_type w2,
  const value_type x3, const value_type y3, const value_type z3, const value_type w3
) noexcept
: _columns{column_type{x0, y0, z0, w0}, column_type{x1, y1, z1, w1}, column_type{x2, y2, z2, w2}, column_type{x3, y3, z3, w3}} { }

template<arithmetic Type>
template<arithmetic From>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(const basic_matrix4x4<From>& other) noexcept
: _columns{column_type{other[0]}, column_type{other[1]}, column_type{other[2]}, column_type{other[3]}} { }

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type> basic_matrix4x4<Type>::transpose(const basic_matrix4x4& matrix) noexcept {
  auto result = basic_matrix4x4<value_type>{};

  result[0][0] = matrix[0][0];
  result[0][1] = matrix[1][0];
  result[0][2] = matrix[2][0];
  result[0][3] = matrix[3][0];

  result[1][0] = matrix[0][1];
  result[1][1] = matrix[1][1];
  result[1][2] = matrix[2][1];
  result[1][3] = matrix[3][1];

  result[2][0] = matrix[0][2];
  result[2][1] = matrix[1][2];
  result[2][2] = matrix[2][2];
  result[2][3] = matrix[3][2];

  result[3][0] = matrix[0][3];
  result[3][1] = matrix[1][3];
  result[3][2] = matrix[2][3];
  result[3][3] = matrix[3][3];

  return result;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type> basic_matrix4x4<Type>::inverse(const basic_matrix4x4& matrix) noexcept {
  const auto coef00 = matrix[2][2] * matrix[3][3] - matrix[3][2] * matrix[2][3];
  const auto coef02 = matrix[1][2] * matrix[3][3] - matrix[3][2] * matrix[1][3];
  const auto coef03 = matrix[1][2] * matrix[2][3] - matrix[2][2] * matrix[1][3];

  const auto coef04 = matrix[2][1] * matrix[3][3] - matrix[3][1] * matrix[2][3];
  const auto coef06 = matrix[1][1] * matrix[3][3] - matrix[3][1] * matrix[1][3];
  const auto coef07 = matrix[1][1] * matrix[2][3] - matrix[2][1] * matrix[1][3];

  const auto coef08 = matrix[2][1] * matrix[3][2] - matrix[3][1] * matrix[2][2];
  const auto coef10 = matrix[1][1] * matrix[3][2] - matrix[3][1] * matrix[1][2];
  const auto coef11 = matrix[1][1] * matrix[2][2] - matrix[2][1] * matrix[1][2];

  const auto coef12 = matrix[2][0] * matrix[3][3] - matrix[3][0] * matrix[2][3];
  const auto coef14 = matrix[1][0] * matrix[3][3] - matrix[3][0] * matrix[1][3];
  const auto coef15 = matrix[1][0] * matrix[2][3] - matrix[2][0] * matrix[1][3];

  const auto coef16 = matrix[2][0] * matrix[3][2] - matrix[3][0] * matrix[2][2];
  const auto coef18 = matrix[1][0] * matrix[3][2] - matrix[3][0] * matrix[1][2];
  const auto coef19 = matrix[1][0] * matrix[2][2] - matrix[2][0] * matrix[1][2];

  const auto coef20 = matrix[2][0] * matrix[3][1] - matrix[3][0] * matrix[2][1];
  const auto coef22 = matrix[1][0] * matrix[3][1] - matrix[3][0] * matrix[1][1];
  const auto coef23 = matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1];

  const auto fac0 = basic_vector4<value_type>{coef00, coef00, coef02, coef03};
  const auto fac1 = basic_vector4<value_type>{coef04, coef04, coef06, coef07};
  const auto fac2 = basic_vector4<value_type>{coef08, coef08, coef10, coef11};
  const auto fac3 = basic_vector4<value_type>{coef12, coef12, coef14, coef15};
  const auto fac4 = basic_vector4<value_type>{coef16, coef16, coef18, coef19};
  const auto fac5 = basic_vector4<value_type>{coef20, coef20, coef22, coef23};

  const auto vec0 = basic_vector4<value_type>{matrix[1][0], matrix[0][0], matrix[0][0], matrix[0][0]};
  const auto vec1 = basic_vector4<value_type>{matrix[1][1], matrix[0][1], matrix[0][1], matrix[0][1]};
  const auto vec2 = basic_vector4<value_type>{matrix[1][2], matrix[0][2], matrix[0][2], matrix[0][2]};
  const auto vec3 = basic_vector4<value_type>{matrix[1][3], matrix[0][3], matrix[0][3], matrix[0][3]};

  const auto inv0 = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
  const auto inv1 = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
  const auto inv2 = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
  const auto inv3 = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;

  const auto sign0 = basic_vector4<value_type>{+1, -1, +1, -1};
  const auto sign1 = basic_vector4<value_type>{-1, +1, -1, +1};

  const auto inverse = basic_matrix4x4<value_type>{inv0 * sign0, inv1 * sign1, inv2 * sign0, inv3 * sign1};

  const auto row0 = basic_vector4<value_type>{inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]};

  const auto dot0 = matrix[0] * row0;
  
  // I dont know why those parantheses are needed here... But im too scared to remove them
  const auto dot1 = value_type{(dot0.x + dot0.y) + (dot0.z + dot0.w)};

  assert(dot1 != value_type{0});

  const auto one_over_determinant = value_type{1} / dot1;

  return inverse * one_over_determinant;
}

template<arithmetic Type>
template<arithmetic From>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator=(const basic_matrix4x4<From>& other) noexcept {
  _columns[0] = column_type{other[0]};
  _columns[1] = column_type{other[1]};
  _columns[2] = column_type{other[2]};
  _columns[3] = column_type{other[3]};

  return *this;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator+=(const basic_matrix4x4<Type>& other) noexcept {
  _columns[0] += other[0];
  _columns[1] += other[1];
  _columns[2] += other[2];
  _columns[3] += other[3];

  return *this;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator-=(const basic_matrix4x4<Type>& other) noexcept {
  _columns[0] -= other[0];
  _columns[1] -= other[1];
  _columns[2] -= other[2];
  _columns[3] -= other[3];

  return *this;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator*=(const Type scalar) noexcept {
  _columns[0] *= scalar;
  _columns[1] *= scalar;
  _columns[2] *= scalar;
  _columns[3] *= scalar;

  return *this;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>::column_type_reference basic_matrix4x4<Type>::operator[](const index_type index) noexcept {
  assert(index < 4);
  return _columns[index];
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>::const_column_type_reference basic_matrix4x4<Type>::operator[](const index_type index) const noexcept {
  assert(index < 4);
  return _columns[index];
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>::pointer basic_matrix4x4<Type>::data() noexcept {
  return &_columns[0].x;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type>::const_pointer basic_matrix4x4<Type>::data() const noexcept {
  return &_columns[0].x;
}

template<arithmetic Type>
constexpr void basic_matrix4x4<Type>::transpose() noexcept {
  *this = transpose(*this);
}

template<arithmetic Type>
constexpr void basic_matrix4x4<Type>::inverse() noexcept {
  *this = inverse(*this);
}

template<arithmetic Type>
inline constexpr bool operator==(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type> operator+(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type> operator-(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type> operator*(basic_matrix4x4<Type> lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<arithmetic Type>
inline constexpr basic_vector4<Type> operator*(basic_matrix4x4<Type> lhs, const basic_vector4<Type>& rhs) noexcept {
  using index_type = basic_matrix4x4<Type>::index_type;

  // [NOTE] KAJ 2022-02-04 23:42 - This might become a performance bottleneck in the future. But most matrix multiplications are going to happen on the GPU anyways.
  auto result = basic_vector4<Type>{};

  for (auto column = index_type{0}; column < 4; ++column) {
    for (auto i = index_type{0}; i < 4; ++i) {
      result[column] += lhs[column][i] * rhs[i];
    }
  }

  return result;
}

template<arithmetic Type>
inline constexpr basic_matrix4x4<Type> operator*(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  using index_type = basic_matrix4x4<Type>::index_type;

  // [NOTE] KAJ 2022-02-04 23:42 - This might become a performance bottleneck in the future. But most matrix multiplications are going to happen on the GPU anyways.
  auto result = basic_matrix4x4<Type>{};

  for (auto column = index_type{0}; column < 4; ++column) {
    for (auto row = index_type{0}; row < 4; ++row) {
      for (auto i = index_type{0}; i < 4; ++i) {
        result[column][row] += lhs[column][i] * rhs[i][row];
      }
    }
  }

  return result;
}

template<arithmetic Type>
inline constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_matrix4x4<Type>& matrix) noexcept {
  using index_type = basic_matrix4x4<Type>::index_type;

  auto default_state = std::ios{nullptr};
  default_state.copyfmt(output_stream);

  output_stream << std::setprecision(4) << std::fixed;

  for (auto column = index_type{0}; column < 4; ++column) {
    output_stream << "| ";

    for (auto row = index_type{0}; row < 4; ++row) {
      output_stream << matrix[column][row] << ' ';
    }

    output_stream << "|\n";
  }
  
  output_stream.copyfmt(default_state);

  return output_stream; 
}

} // namespace sbx