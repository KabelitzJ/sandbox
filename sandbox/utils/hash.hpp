#ifndef SBX_UTILS_HASH_HPP_
#define SBX_UTILS_HASH_HPP_

#include <cstddef>

namespace sbx {

inline void hash_combine(std::size_t& seed, std::size_t hash) {
  hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= hash;
}

} // namespace sbx

#endif // SBX_UTILS_HASH_HPP_
