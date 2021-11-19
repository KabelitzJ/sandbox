#ifndef SBX_UTILS_HASH_HPP_
#define SBX_UTILS_HASH_HPP_

#include <cstddef>
#include <utility>

namespace sbx {

template<typename Type, typename Hash = std::hash<Type>>
inline void hash_combine(std::size_t& seed, const Type& type) {
  auto hasher = Hash{};

  seed ^= hasher(type) +  0x9e3779b9 + (seed << 6) + (seed >> 2);
}

} // namespace sbx

#endif // SBX_UTILS_HASH_HPP_
