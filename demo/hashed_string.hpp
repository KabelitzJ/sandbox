#ifndef DEMO_HASHED_STRING_HPP_
#define DEMO_HASHED_STRING_HPP_

#include <string>
#include <string_view>

#include "hash.hpp"

namespace demo {

template<character Character>
class basic_hashed_string { 

public:

  using char_type = Character;
  using size_type = std::size_t;
  using hash_type = std::size_t;

  constexpr basic_hashed_string() noexcept
  : _hash{0} { }

  constexpr basic_hashed_string(const char_type* string, const size_type size) noexcept
  : _hash{fnv_1a_hash(string, size)} { }

  template<size_type Size>
  constexpr basic_hashed_string(const char_type(&string)[Size]) noexcept
  : _hash{fnv_1a_hash(string, Size)} { }

  constexpr basic_hashed_string(const std::basic_string<char_type>& string) noexcept
  : _hash{fnv_1a_hash(string.data(), string.size())} { }

  constexpr basic_hashed_string(std::basic_string_view<char_type> string) noexcept
  : _hash{fnv_1a_hash(string.data(), string.size())} { }

  ~basic_hashed_string() noexcept = default;

  constexpr operator hash_type() const noexcept {
    return _hash;
  }

  constexpr hash_type hash() const noexcept {
    return _hash;
  }

private:

  hash_type _hash{};

}; // class basic_hashed_string

template<character Character>
[[nodiscard]] constexpr bool operator==(const basic_hashed_string<Character>& lhs, const basic_hashed_string<Character>& rhs) noexcept {
  return lhs.hash() == rhs.hash();
}

template<character Character>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const basic_hashed_string<Character>& lhs, const basic_hashed_string<Character>& rhs) noexcept {
  return lhs.hash() <=> rhs.hash();
}

template<character Character>
basic_hashed_string(const Character* string, const std::size_t size) -> basic_hashed_string<Character>;

template<character Character, std::size_t Size>
basic_hashed_string(const Character(&string)[Size]) -> basic_hashed_string<Character>;

template<character Character, std::size_t Size>
basic_hashed_string(const std::basic_string<Character>&) -> basic_hashed_string<Character>;

template<character Character, std::size_t Size>
basic_hashed_string(std::basic_string_view<Character>) -> basic_hashed_string<Character>;

using hashed_string = basic_hashed_string<char>;

namespace literals {

[[nodiscard]] constexpr hashed_string operator""_hashed_string(const char* string, const std::size_t size) noexcept {
  return hashed_string{string, size};
}

} // namespace literals

} // namespace demo

template<>
struct std::hash<demo::hashed_string> {
  [[nodiscard]] constexpr std::size_t operator()(const demo::hashed_string& string) const noexcept {
    return string.hash();
  }
};

#endif // DEMO_HASHED_STRING_HPP_
