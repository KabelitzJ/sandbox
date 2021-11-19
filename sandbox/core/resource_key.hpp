#ifndef SBX_CORE_RESOURCE_KEY_HPP_
#define SBX_CORE_RESOURCE_KEY_HPP_

#include <string>
#include <utility>

#include <types/primitives.hpp>

#include <utils/hash.hpp>

namespace sbx {

struct resource_key {
  uint32 id{};
  std::string name{};
}; // struct resource_key

} // namespace sbx

template<>
struct std::hash<sbx::resource_key> {
  std::size_t operator()(const sbx::resource_key& key) const {
    auto seed = std::size_t{0u};

    sbx::hash_combine(seed, key.id);
    sbx::hash_combine(seed, key.name);

    return seed;
  }
};

template<>
struct std::equal_to<sbx::resource_key> {
  bool operator()(const sbx::resource_key& lhs, const sbx::resource_key& rhs) const {
    return lhs.id == rhs.id && lhs.name == rhs.name;
  }
};

#endif // SBX_CORE_RESOURCE_KEY_HPP_
