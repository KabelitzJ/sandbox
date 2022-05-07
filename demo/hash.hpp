#ifndef DEMO_HASH_HPP_
#define DEMO_HASH_HPP_

#include <concepts>
#include <string>

namespace demo {

// template<std::size_t Prime = 16777619u, std::size_t OffsetBasis = 2166136261u>
// struct fnv_1a_hash {

//   [[nodiscard]] constexpr std::size_t operator()(const std::string& string) const noexcept {
//     std::size_t hash = OffsetBasis;

//     for (const auto& character : string) {
//       hash ^= character;
//       hash *= Prime;
//     }

//     return hash;
//   }

// };

template<typename Type>
concept character = std::is_same_v<Type, char> || (sizeof(Type) == 1 && std::is_convertible_v<Type, char>);

template<character Character>
requires (sizeof(Character) == 1)
[[nodiscard]] constexpr std::size_t fnv_1a_hash(const Character* string, std::size_t size) noexcept {
  auto hash = std::size_t{2166136261};

  for (auto i = std::size_t{0}; i < size; ++i) {
    hash ^= static_cast<std::size_t>(string[i]);
    hash *= std::size_t{16777619};
  }

  return hash;
}

} // namespace demo

#endif // DEMO_HASH_HPP_
