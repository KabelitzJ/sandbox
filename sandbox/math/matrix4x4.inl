#include <cassert>
#include <algorithm>
#include <iomanip>

namespace sbx {

template<typename Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4() noexcept
: _columns{} { }

template<typename Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(const std::array<column_type, 4>& columns) noexcept
: _columns{columns} { }

template<typename Type>
inline constexpr basic_matrix4x4<Type>::basic_matrix4x4(std::initializer_list<value_type> values) noexcept
: _columns{} {
  assert(values.size() == 16);

  std::copy_n(values.begin(), 16, data());
}

template<typename Type>
inline constexpr basic_matrix4x4<Type> basic_matrix4x4<Type>::identity() noexcept {
  return basic_matrix4x4<Type>{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };
}

template<typename Type>
inline constexpr typename basic_matrix4x4<Type>::column_type_reference basic_matrix4x4<Type>::operator[](const index_type index) noexcept {
  return _columns[index];
}

template<typename Type>
inline constexpr typename basic_matrix4x4<Type>::const_column_type_reference basic_matrix4x4<Type>::operator[](const index_type index) const noexcept {
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
inline constexpr std::ostream& operator<<(std::ostream& output_stream, const basic_matrix4x4<Type>& matrix) noexcept {
  auto default_state = std::ios{nullptr};
  default_state.copyfmt(output_stream);

  output_stream << std::setprecision(4) << std::fixed;

  for (auto row = typename basic_matrix4x4<Type>::index_type{0}; row < 4; ++row) {
    output_stream << "| ";

    for (auto column = typename basic_matrix4x4<Type>::index_type{0}; column < 4; ++column) {
      output_stream << matrix[column][row] << ' ';
    }

    output_stream << "|\n";
  }
  
  output_stream.copyfmt(default_state);

  return output_stream; 
}

} // namespace sbx