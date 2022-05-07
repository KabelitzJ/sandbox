#ifndef DEMO_HASHED_STRING_HPP_
#define DEMO_HASHED_STRING_HPP_

#include <string>

#include "hash.hpp"

namespace demo {

template<character Character>
class basic_hashed_string { 

  template<typename>
  friend constexpr bool operator==(const basic_hashed_string<Character>& lhs, const basic_hashed_string<Character>& rhs) noexcept;
  template<typename>
  friend constexpr bool operator<=>(const basic_hashed_string<Character>& lhs, const basic_hashed_string<Character>& rhs) noexcept;

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

  ~basic_hashed_string() noexcept = default;

  constexpr operator hash_type() const noexcept {
    return _hash;
  }

private:

  hash_type _hash{};

}; // class basic_hashed_string

template<character Character>
[[nodiscard]] constexpr bool operator==(const basic_hashed_string<Character>& lhs, const basic_hashed_string<Character>& rhs) noexcept {
  return lhs._hash == rhs._hash;
}

template<character Character>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const basic_hashed_string<Character>& lhs, const basic_hashed_string<Character>& rhs) noexcept {
  return lhs._hash <=> rhs._hash;
}

template<character Character>
basic_hashed_string(const Character* string, const std::size_t size) -> basic_hashed_string<Character>;

template<character Character, std::size_t Size>
basic_hashed_string(const Character(&string)[Size]) -> basic_hashed_string<Character>;

using hashed_string = basic_hashed_string<char>;

namespace literals {

[[nodiscard]] constexpr hashed_string operator""_hashed_string(const char* string, const std::size_t size) noexcept {
  return hashed_string{string, size};
}

} // namespace literals

} // namespace demo

#endif // DEMO_HASHED_STRING_HPP_
