#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <types/primitives.hpp>

#include "entity_traits.hpp"

namespace sbx {

enum class default_entity : uint32 { };

struct null_entity_t {

  template<entity Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept {
    using traits = entity_traits<Entity>;
    return traits::combine(traits::reserved, traits::reserved);
  }

}; // struct null_entity_t

constexpr bool operator==([[maybe_unused]] const null_entity_t lhs, [[maybe_unused]] const null_entity_t rhs) noexcept {
  return true;
}

template<entity Entity>
constexpr bool operator==(const Entity lhs, const null_entity_t rhs) noexcept {
  using traits = entity_traits<Entity>;
  return traits::to_entity(lhs) == traits::to_entity(rhs);
}

inline constexpr null_entity_t null_entity{};

struct tombstone_entity_t {

  template<entity Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept {
    using traits = entity_traits<Entity>;
    return traits::combine(traits::reserved, traits::reserved);
  }

}; // struct tombstone_entity_t

constexpr bool operator==([[maybe_unused]] const tombstone_entity_t lhs, [[maybe_unused]] const tombstone_entity_t rhs) noexcept {
  return true;
}

template<entity Entity>
constexpr bool operator==(const Entity lhs, const tombstone_entity_t rhs) noexcept {
  using traits = entity_traits<Entity>;
  return traits::to_version(lhs) == traits::to_version(rhs);
}

inline constexpr tombstone_entity_t tombstone_entity{};

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
