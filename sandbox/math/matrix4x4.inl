#include <cassert>
#include <algorithm>
#include <iomanip>

namespace sbx {

template<typename Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4() noexcept
: _columns{column_type{0}, column_type{0}, column_type{0}, column_type{0}} { }

template<typename Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(const std::array<column_type, 4>& columns) noexcept
: _columns{columns} { }

template<typename Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(
  const column_type& column0,
  const column_type& column1,
  const column_type& column2,
  const column_type& column3
) noexcept
: _columns{column0, column1, column2, column3} { }

template<typename Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(
  const value_type x0, const value_type y0, const value_type z0, const value_type w0,
  const value_type x1, const value_type y1, const value_type z1, const value_type w1,
  const value_type x2, const value_type y2, const value_type z2, const value_type w2,
  const value_type x3, const value_type y3, const value_type z3, const value_type w3
) noexcept
: _columns{column_type{x0, y0, z0, w0}, column_type{x1, y1, z1, w1}, column_type{x2, y2, z2, w2}, column_type{x3, y3, z3, w3}} { }

template<typename Type>
template<typename From>
requires arithmetic<From> && std::convertible_to<From, Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(const basic_matrix4x4<From>& other) noexcept
: _columns{column_type{other[0]}, column_type{other[1]}, column_type{other[2]}, column_type{other[3]}} { }

template<typename Type>
template<typename From>
requires arithmetic<From> && std::convertible_to<From, Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator=(const basic_matrix4x4<From>& other) noexcept {
  if (*this != other) {
    _columns[0] = column_type{other[0]};
    _columns[1] = column_type{other[1]};
    _columns[2] = column_type{other[2]};
    _columns[3] = column_type{other[3]};
  }

  return *this;
}

template<typename Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator+=(const basic_matrix4x4<Type>& other) noexcept {
  _columns[0] += other[0];
  _columns[1] += other[1];
  _columns[2] += other[2];
  _columns[3] += other[3];

  return *this;
}

template<typename Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator-=(const basic_matrix4x4<Type>& other) noexcept {
  _columns[0] -= other[0];
  _columns[1] -= other[1];
  _columns[2] -= other[2];
  _columns[3] -= other[3];

  return *this;
}

template<typename Type>
inline constexpr basic_matrix4x4<Type>& basic_matrix4x4<Type>::operator*=(const Type scalar) noexcept {
  _columns[0] *= scalar;
  _columns[1] *= scalar;
  _columns[2] *= scalar;
  _columns[3] *= scalar;

  return *this;
}

template<typename Type>
inline constexpr typename basic_matrix4x4<Type>::column_type_reference basic_matrix4x4<Type>::operator[](const index_type index) noexcept {
  assert(index < 4);
  return _columns[index];
}

template<typename Type>
inline constexpr typename basic_matrix4x4<Type>::const_column_type_reference basic_matrix4x4<Type>::operator[](const index_type index) const noexcept {
  assert(index < 4);
  return _columns[index];
}

template<typename Type>
inline constexpr typename basic_matrix4x4<Type>::pointer basic_matrix4x4<Type>::data() noexcept {
  return &_columns[0].x;
}

template<typename Type>
inline constexpr typename basic_matrix4x4<Type>::const_pointer basic_matrix4x4<Type>::data() const noexcept {
  return &_columns[0].x;
}

template<typename Type>
inline constexpr bool operator==(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
}

template<typename Type>
inline constexpr bool operator!=(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return !(lhs == rhs);
}

template<typename Type>
inline constexpr basic_matrix4x4<Type> operator+(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs += rhs;
}

template<typename Type>
inline constexpr basic_matrix4x4<Type> operator-(const basic_matrix4x4<Type>& lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  return lhs -= rhs;
}

template<typename Type>
inline constexpr basic_matrix4x4<Type> operator*(const basic_matrix4x4<Type>& lhs, const Type rhs) noexcept {
  return lhs *= rhs;
}

template<typename Type>
inline constexpr basic_vector4<Type> operator*(basic_matrix4x4<Type> lhs, const basic_vector4<Type>& rhs) noexcept {
  // return basic_vector4<Type>{
  //   lhs[0][0] * rhs[0] + lhs[1][0] * rhs[1] + lhs[2][0] * rhs[2] + lhs[3][0] * rhs[3],
  //   lhs[0][1] * rhs[0] + lhs[1][1] * rhs[1] + lhs[2][1] * rhs[2] + lhs[3][1] * rhs[3],
  //   lhs[0][2] * rhs[0] + lhs[1][2] * rhs[1] + lhs[2][2] * rhs[2] + lhs[3][2] * rhs[3],
  //   lhs[0][3] * rhs[0] + lhs[1][3] * rhs[1] + lhs[2][3] * rhs[2] + lhs[3][3] * rhs[3]
  // };

  // using column_type = typename basic_matrix4x4<Type>::column_type;

  // const auto mov0 = column_type{rhs[0]};
  // const auto mov1 = column_type{rhs[1]};
  // const auto mul0 = column_type{lhs[0] * mov0};
  // const auto mul1 = column_type{lhs[1] * mov1};
  // const auto add0 = column_type{mul0 + mul1};
  // const auto mov2 = column_type{rhs[2]};
  // const auto mov3 = column_type{rhs[3]};
  // const auto mul2 = column_type{lhs[2] * mov2};
  // const auto mul3 = column_type{lhs[3] * mov3};
  // const auto add1 = column_type{mul2 + mul3};
  // const auto add2 = column_type{add0 + add1};

  // return add2;

  using index_type = typename basic_matrix4x4<Type>::index_type;

  // [NOTE] KAJ 2022-02-04 23:42 - This might become a performance bottleneck in the future. But most matrix multiplications are going to happen on the GPU anyways.
  auto result = basic_vector4<Type>{};

  for (auto column = index_type{0}; column < 4; ++column) {
    for (auto i = index_type{0}; i < 4; ++i) {
      result[column] += lhs[column][i] * rhs[i];
    }
  }

  return result;
}

template<typename Type>
inline constexpr basic_matrix4x4<Type> operator*(basic_matrix4x4<Type> lhs, const basic_matrix4x4<Type>& rhs) noexcept {
  // return basic_matrix4x4<Type>{
  //   lhs[0] * rhs[0][0] + lhs[1] * rhs[0][1] + lhs[2] * rhs[0][2] + lhs[3] * rhs[0][3],
  //   lhs[0] * rhs[1][0] + lhs[1] * rhs[1][1] + lhs[2] * rhs[1][2] + lhs[3] * rhs[1][3],
  //   lhs[0] * rhs[2][0] + lhs[1] * rhs[2][1] + lhs[2] * rhs[2][2] + lhs[3] * rhs[2][3],
  //   lhs[0] * rhs[3][0] + lhs[1] * rhs[3][1] + lhs[2] * rhs[3][2] + lhs[3] * rhs[3][3]
  // };

  // using column_type = typename basic_matrix4x4<Type>::column_type;

  // const auto lhs0 = column_type{lhs[0]};
  // const auto lhs1 = column_type{lhs[1]};
  // const auto lhs2 = column_type{lhs[2]};
  // const auto lhs3 = column_type{lhs[3]};

  // const auto rhs0 = column_type{rhs[0]};
  // const auto rhs1 = column_type{rhs[1]};
  // const auto rhs2 = column_type{rhs[2]};
  // const auto rhs3 = column_type{rhs[3]};

  // auto result = basic_matrix4x4<Type>{};
  // result[0] = lhs0 * rhs0[0] + lhs1 * rhs0[1] + lhs2 * rhs0[2] + lhs3 * rhs0[3];
  // result[1] = lhs0 * rhs1[0] + lhs1 * rhs1[1] + lhs2 * rhs1[2] + lhs3 * rhs1[3];
  // result[2] = lhs0 * rhs2[0] + lhs1 * rhs2[1] + lhs2 * rhs2[2] + lhs3 * rhs2[3];
  // result[3] = lhs0 * rhs3[0] + lhs1 * rhs3[1] + lhs2 * rhs3[2] + lhs3 * rhs3[3];
  // return result;

  using index_type = typename basic_matrix4x4<Type>::index_type;

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

template<typename Type>
inline constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_matrix4x4<Type>& matrix) noexcept {
  using index_type = typename basic_matrix4x4<Type>::index_type;

  auto default_state = std::ios{nullptr};
  default_state.copyfmt(output_stream);

  output_stream << std::setprecision(4) << std::fixed;

  for (auto row = index_type{0}; row < 4; ++row) {
    output_stream << "| ";

    for (auto column = index_type{0}; column < 4; ++column) {
      output_stream << matrix[column][row] << ' ';
    }

    output_stream << "|\n";
  }
  
  output_stream.copyfmt(default_state);

  return output_stream; 
}

} // namespace sbx