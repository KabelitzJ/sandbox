/* 
 * Copyright (c) 2022 Jonas Kabelitz
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * You should have received a copy of the MIT License along with this program.
 * If not, see <https://opensource.org/licenses/MIT/>.
 */

/**
 * @file libsbx/utility/hash.hpp
 */

#ifndef LIBSBX_UTILITY_HASH_HPP_
#define LIBSBX_UTILITY_HASH_HPP_

/**
 * @ingroup libsbx-utility
 */

#include <utility>
#include <string>
#include <string_view>
#include <iostream>
#include <concepts>

namespace sbx::utility {

/**
 * @brief A concept that represents a type that can be hashed.
 */
template<typename Type>
concept hashable = requires(const Type& instance) {
  { std::hash<Type>{}(instance) } -> std::same_as<std::size_t>;
}; // concept hashable

/**
 * @brief Combines multiple hashes into a single hash.
 */
inline constexpr auto hash_combine(std::size_t& seed) -> void { }

/**
 * @brief Combines multiple hashes into a single hash.
 * 
 * @tparam Type The type of the first value to hash.
 * @tparam Rest The types of the remaining values to hash.
 * 
 * @param seed The seed to combine the hashes with.
 * @param value The first value to hash.
 * @param rest The remaining values to hash.
 */
template<hashable Type, hashable... Rest>
inline constexpr auto hash_combine(std::size_t& seed, const Type& value, Rest... rest) -> void {
  auto hasher = std::hash<Type>{};
  seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hash_combine(seed, rest), ...);
}

/**
 * @brief A concept that represents a character type.
 * 
 * @tparam Type The type to check.
 */
template<typename Type>
concept character = std::same_as<Type, char> || std::same_as<Type, wchar_t> || std::same_as<Type, char8_t> || std::same_as<Type, char16_t> || std::same_as<Type, char32_t>;

/**
 * @brief Traits for the fnv1a hash function.
 * 
 * @tparam Type The type of the hash.
 */
template<std::unsigned_integral Type>
struct fnv1a_traits;

/**
 * @brief Traits for the fnv1a hash function specialized for 32-bit hashes.
 * 
 * @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV_hash_parameters
 */
template<>
struct fnv1a_traits<std::uint32_t> {
  /** @brief The offset basis for 32-bit fnv1a. */
  inline static constexpr auto offset_basis = std::uint32_t{0x811c9dc5};
  /** @brief The prime for 32-bit fnv1a. */
  inline static constexpr auto prime = std::uint32_t{0x01000193};
}; // struct fnv1a_traits

/**
 * @brief Traits for the fnv1a hash function specialized for 64-bit hashes.
 * 
 * @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV_hash_parameters
 */
template<>
struct fnv1a_traits<std::uint64_t> {
  /** @brief The offset basis for 64-bit fnv1a. */
  inline static constexpr auto offset_basis = std::uint64_t{0xcbf29ce484222325};
  /** @brief The prime for 64-bit fnv1a. */
  inline static constexpr auto prime = std::uint64_t{0x00000100000001B3};
}; // struct fnv1a_traits

/**
 * @brief Functor that implements the fnv1a hash algorithm.
 * 
 * @tparam Char The character type of the string to hash.
 * @tparam Hash The type of the hash. Defaults to std::uint64_t.
 * @tparam HashTraits The traits of the hash. Defaults to fnv1a_traits<Hash>.
 */
template<character Char, std::unsigned_integral Hash = std::uint64_t, typename HashTraits = fnv1a_traits<Hash>>
struct fnv1a_hash {
  using char_type = Char;
  using size_type = std::size_t;
  using hash_type = Hash;
  using hash_traits = HashTraits;

  /**
   * @brief Hashes the given string.
   * 
   * @param string The string to hash. 
   * @param size The size of the string.
   * 
   * @return hash_type The hash of the string.
   */
  inline constexpr auto operator()(std::basic_string_view<Char> string) const noexcept -> hash_type {
    auto hash = hash_traits::offset_basis;

    for (const auto& character : string) {
      hash ^= static_cast<hash_type>(character);
      hash *= hash_traits::prime;
    }

    return hash;
  }
}; // struct fnv1a_hash

/**
 * @brief A concept that represents a hash function that hashes a string.
 * 
 * @tparam Type The type of the hash function.
 * @tparam Char The character type of the string to hash.
 */
template<typename Type, typename Char>
concept string_hash = requires(const Type& instance, std::basic_string_view<Char> string) {
  { instance.operator()(string) } -> std::unsigned_integral;
}; // concept string_hash

/**
 * @brief A hashed string.
 * 
 * @tparam Char The character type of the string.
 * @tparam HashFunction The hash function to use. Defaults to fnv1a_hash<Char>.
 */
template<character Char, string_hash<Char> HashFunction = fnv1a_hash<Char>>
class basic_hashed_string {

public:

  using char_type = Char;
  using size_type = std::size_t;
  using hash_type = std::size_t;
  using hash_function = HashFunction;

  /**
   * @brief Default constructor. Initializes the hash to 0.
   */
  inline constexpr basic_hashed_string() noexcept = default;

  /**
   * @brief Constructs a hashed string from the given string.
   * 
   * @param string The string to hash.
   */
  inline constexpr basic_hashed_string(std::basic_string_view<char_type> string)
  : _hash{hash_function{}(string)} { }

  /**
   * @brief Destructor.
   */
  ~basic_hashed_string() = default;

  /**
   * @brief Returns the hash of the string.
   * 
   * @return hash_type The hash of the string.
   */
  inline constexpr auto hash() const noexcept -> hash_type {
    return _hash;
  }

  /**
   * @brief Returns the hash of the string.
   * 
   * @return hash_type The hash of the string.
   */
  inline constexpr operator std::size_t() const noexcept {
    return _hash;
  }

private:

  // // [NOTE] KAJ 2023-02-21 01:46 - If we ever decide to save the string, we have to create a copy of it.
  hash_type _hash{};

}; // class basic_hashed_string

/**
 * @brief Compares two hashed strings for equality.
 * 
 * @tparam Char The character type of the strings.
 * @tparam HashFunction The hash function used to hash the strings.
 */
template<character Char, typename HashFunction = fnv1a_hash<Char>>
inline constexpr auto operator==(const basic_hashed_string<Char, HashFunction>& lhs, const basic_hashed_string<Char, HashFunction>& rhs) -> bool {
  return lhs == rhs;
}

/**
 * @brief Specialization of basic_hashed_string for char.
 */
using hashed_string = basic_hashed_string<char>;

/**
 * @brief Specialization of basic_hashed_string for wchar_t.
 */
using hashed_wstring = basic_hashed_string<wchar_t>;

/**
 * @brief Specialization of basic_hashed_string for char8_t.
 */
using u8hashed_string = basic_hashed_string<char8_t>;

/**
 * @brief Specialization of basic_hashed_string for char16_t.
 */
using u16hashed_string = basic_hashed_string<char16_t>;

/**
 * @brief Specialization of basic_hashed_string for char32_t.
 */
using u32hashed_string = basic_hashed_string<char32_t>;

namespace literals {

/**
 * @brief Creates a hashed string from the given string literal.
 */
inline constexpr auto operator""_hs(const char* string, std::size_t size) -> hashed_string {
  return hashed_string{std::string_view{string, size}};
}

/**
 * @brief Creates a hashed wstring from the given string literal.
 */
inline constexpr auto operator""_hs(const wchar_t* string, std::size_t size) -> hashed_wstring {
  return hashed_wstring{std::wstring_view{string, size}};
}

/**
 * @brief Creates a hashed u8string from the given string literal.
 */
inline constexpr auto operator""_hs(const char8_t* string, std::size_t size) -> u8hashed_string {
  return u8hashed_string{std::u8string_view{string, size}};
}

/**
 * @brief Creates a hashed u16string from the given string literal.
 */
inline constexpr auto operator""_hs(const char16_t* string, std::size_t size) -> u16hashed_string {
  return u16hashed_string{std::u16string_view{string, size}};
}

/**
 * @brief Creates a hashed u32string from the given string literal.
 */
inline constexpr auto operator""_hs(const char32_t* string, std::size_t size) -> u32hashed_string {
  return u32hashed_string{std::u32string_view{string, size}};
}

} // namespace literals

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_HASH_HPP_
