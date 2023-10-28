#ifndef LIBSBX_UTILITY_STRING_LITERAL_HPP_
#define LIBSBX_UTILITY_STRING_LITERAL_HPP_

#include <utility>
#include <string_view>
#include <array>
#include <limits>

namespace sbx::utility {

namespace detail {

template<std::forward_iterator InputIterator, std::sentinel_for<InputIterator> Sentinel, std::forward_iterator OutputIterator>
constexpr auto copy(InputIterator first, Sentinel last, OutputIterator result) -> OutputIterator {
  while (first != last) {
    *result++ = *first++;
  }

  return result;
}

} // namespace detail

template<typename Character, std::size_t Size>
class basic_string_literal {

public:

  using character_type = Character;
  using size_type = std::size_t;
  using iterator = const character_type*;

  static constexpr auto npos = std::numeric_limits<size_type>::max();

  constexpr basic_string_literal(const character_type (&data)[Size]) noexcept {
    detail::copy(data, data + Size - 1, _data.data());
  }

  constexpr auto begin() const noexcept -> iterator {
    return std::begin(_data);
  }

  constexpr auto end() const noexcept -> iterator {
    return std::end(_data.data());
  }

  constexpr auto data() const noexcept -> const character_type* {
    return _data.data();
  }

  constexpr auto size() const noexcept -> size_type {
    return Size - 1;
  }

  constexpr auto is_empty() const noexcept -> bool {
    return size() == 0;
  }

  constexpr auto operator[](size_type index) const noexcept -> character_type {
    return _data[index];
  }

  constexpr operator std::basic_string_view<character_type>() const noexcept {
    return std::basic_string_view<character_type>{_data.data(), Size};
  }

  std::array<character_type, Size - 1> _data;

}; // class basic_string_literal

template<std::size_t Size>
using string_literal = basic_string_literal<char, Size>;

template<std::size_t Size>
using wstring_literal = basic_string_literal<wchar_t, Size>;

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_STRING_LITERAL_HPP_
