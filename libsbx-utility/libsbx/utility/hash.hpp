#ifndef LIBSBX_CORE_HASH_HPP_
#define LIBSBX_CORE_HASH_HPP_

#include <utility>

namespace sbx::core {

template<typename Type>
concept hashable = requires(const Type& instance) {
  { std::hash<Type>{}(instance) } -> std::same_as<std::size_t>;
}; // concept hashable

inline constexpr auto hash_combine(std::size_t& seed) -> void { }

template<hashable Type, hashable... Rest>
inline constexpr auto hash_combine(std::size_t& seed, const Type& value, Rest... rest) -> void {
  auto hasher = std::hash<Type>{};
  seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hash_combine(seed, rest), ...);
}


} // namespace sbx::core

#endif // LIBSBX_CORE_HASH_HPP_
