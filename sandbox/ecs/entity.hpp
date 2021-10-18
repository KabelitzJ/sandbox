#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

template<typename, typename = void>
struct is_entity_type : std::false_type { };

template<typename Entity>
struct is_entity_type<Entity, std::enable_if_t<std::is_same_v<Entity, uint32>>> : std::true_type { };

template<typename Entity>
struct is_entity_type<Entity, std::enable_if_t<std::is_enum_v<Entity>>> : is_entity_type<Entity, std::underlying_type_t<Entity>> { };

template<typename Entity>
inline constexpr auto is_entity_type_v = is_entity_type<Entity>::value;

template<typename, typename = void>
struct entity_traits;

template<typename Entity>
struct entity_traits<Entity, std::enable_if_t<is_entity_type_v<Entity>>> {

  using value_type = Entity;

  using entity_type = uint32;
  using version_type = uint16;

  static constexpr auto entity_mask = entity_type{0xFFFFF};
  static constexpr auto version_mask = entity_type{0xFFF};
  static constexpr auto entity_shift = std::size_t{20u};
  static constexpr auto reserved = entity_mask | (version_mask << entity_shift);

  [[nodiscard]] static constexpr entity_type to_integral(const value_type value) noexcept {
    return static_cast<entity_type>(value);
  }

  [[nodiscard]] static constexpr entity_type to_entity(const value_type value) noexcept {
    return (to_integral(value) & entity_mask);
  }

  [[nodiscard]] static constexpr version_type to_version(const value_type value) noexcept {
    constexpr auto mask = (version_mask << entity_shift);
    return ((to_integral(value) & mask) >> entity_shift);
  }

  [[nodiscard]] static constexpr value_type construct(const entity_type entity, const version_type version) noexcept {
    return value_type{(entity & entity_mask) | (static_cast<entity_type>(version) << entity_shift)};
  }

  [[nodiscard]] static constexpr value_type combine(const entity_type lhs, const entity_type rhs) noexcept {
    constexpr auto mask = (version_mask << entity_shift);
    return value_type{(lhs & entity_mask) | (rhs & mask)};
  }

};

template<typename Entity>
[[nodiscard]] constexpr typename entity_traits<Entity>::entity_type to_integral(const Entity value) noexcept {
  return entity_traits<Entity>::to_integral(value);
}

template<typename Entity>
[[nodiscard]] constexpr typename entity_traits<Entity>::entity_type to_entity(const Entity value) noexcept {
  return entity_traits<Entity>::to_entity(value);
}

template<typename Entity>
[[nodiscard]] constexpr typename entity_traits<Entity>::version_type to_version(const Entity value) noexcept {
  return entity_traits<Entity>::to_version(value);
}


enum class entity : uint32 { };


struct null_entity_t {

  template<typename Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept {
    using entity_traits = sbx::entity_traits<Entity>;
    return entity_traits::combine(entity_traits::reserved, entity_traits::reserved);
  }

  [[nodiscard]] constexpr bool operator==([[maybe_unused]] const null_entity_t other) const noexcept {
    return true;
  }

  [[nodiscard]] constexpr bool operator!=([[maybe_unused]] const null_entity_t other) const noexcept {
    return false;
  }

  template<typename Entity>
  [[nodiscard]] constexpr bool operator==(const Entity entity) const noexcept {
    using traits = entity_traits<Entity>;
    return traits::to_entity(entity) == traits::to_entity(*this);
  }

  template<typename Entity>
  [[nodiscard]] constexpr bool operator!=(const Entity entity) const noexcept {
    return !(entity == *this);
  }

}; // struct null_entity_t

template<typename Entity>
[[nodiscard]] constexpr bool operator==(const Entity entity, const null_entity_t other) noexcept {
  return other.operator==(entity);
}

template<typename Entity>
[[nodiscard]] constexpr bool operator!=(const Entity entity, const null_entity_t other) noexcept {
  return !(other == entity);
}

inline constexpr null_entity_t null_entity{};


struct tombstone_entity_t {

  template<typename Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept {
    using entity_traits = sbx::entity_traits<Entity>;
    return entity_traits::combine(entity_traits::reserved, entity_traits::reserved);
  }

  [[nodiscard]] constexpr bool operator==([[maybe_unused]] const tombstone_entity_t other) const noexcept {
    return true;
  }

  
  [[nodiscard]] constexpr bool operator!=([[maybe_unused]] const tombstone_entity_t other) const noexcept {
    return false;
  }

  template<typename Entity>
  [[nodiscard]] constexpr bool operator==(const Entity entity) const noexcept {
    using entity_traits = sbx::entity_traits<Entity>;
    return entity_traits::to_version(entity) == entity_traits::to_version(*this);
  }

  template<typename Entity>
  [[nodiscard]] constexpr bool operator!=(const Entity entity) const noexcept {
    return !(entity == *this);
  }

}; // struct tombstone_t

template<typename Entity>
[[nodiscard]] constexpr bool operator==(const Entity entity, const tombstone_entity_t other) noexcept {
  return other.operator==(entity);
}

template<typename Entity>
[[nodiscard]] constexpr bool operator!=(const Entity entity, const tombstone_entity_t other) noexcept {
  return !(other == entity);
}

inline constexpr tombstone_entity_t tombstone_entity{};

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
