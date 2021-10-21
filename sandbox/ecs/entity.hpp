#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

template<typename, typename = void>
struct basic_entity_traits;

template<typename Type>
struct basic_entity_traits<Type, std::enable_if_t<std::is_enum_v<Type>>>
: basic_entity_traits<std::underlying_type_t<Type>> { };

template<typename Type>
struct basic_entity_traits<Type, std::enable_if_t<std::is_class_v<Type>>>
: basic_entity_traits<typename Type::entity_type> { };

template<>
struct basic_entity_traits<uint32> {

  using entity_type = uint32;
  using version_type = uint16;

  static constexpr auto entity_mask = entity_type{0xFFFFF};
  static constexpr auto version_mask = entity_type{0xFFF};
  static constexpr auto entity_shift = std::size_t{20u};

};

template<typename Type>
class entity_traits {

  using basic_traits = basic_entity_traits<Type>;

public:

  using value_type = Type;
  using entity_type = typename basic_traits::entity_type;
  using version_type = typename basic_traits::version_type;

  static constexpr auto reserved = basic_traits::entity_mask | (basic_traits::version_mask << basic_traits::entity_shift);

  [[nodiscard]] static constexpr entity_type to_integral(const value_type value) noexcept {
    return static_cast<entity_type>(value);
  }

  [[nodiscard]] static constexpr entity_type to_entity(const value_type value) noexcept {
    return (to_integral(value) & basic_traits::entity_mask);
  }

  [[nodiscard]] static constexpr version_type to_version(const value_type value) noexcept {
    constexpr auto mask = (basic_traits::version_mask << basic_traits::entity_shift);
    return ((to_integral(value) & mask) >> basic_traits::entity_shift);
  }

  [[nodiscard]] static constexpr value_type construct(const entity_type entity, const version_type version) noexcept {
    return value_type{(entity & basic_traits::entity_mask) | (static_cast<entity_type>(version) << basic_traits::entity_shift)};
  }

  [[nodiscard]] static constexpr value_type combine(const entity_type lhs, const entity_type rhs) noexcept {
    constexpr auto mask = (basic_traits::version_mask << basic_traits::entity_shift);
    return value_type{(lhs & basic_traits::entity_mask) | (rhs & mask)};
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

/**
 * @brief Default entity type
 */
enum class entity : uint32 { };

/**
 * @brief Null representation of an entity
 */
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

/**
 * @brief Placeholder entity
 */
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
