#ifndef SBX_UTILS_HASH_HPP_
#define SBX_UTILS_HASH_HPP_

#include <concepts>
#include <string>

namespace sbx {

template<typename Type>
inline void hash_combine(std::size_t& seed, const Type& value) noexcept {
  seed ^= std::hash<Type>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// [TODO] KAJ 2022-07-11 17:45 - This does not support wide chars
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

} // namespace sbx

#endif // SBX_UTILS_HASH_HPP_
