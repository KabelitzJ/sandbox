#ifndef SBX_ECS_ENTITY_TRAITS_HPP_
#define SBX_ECS_ENTITY_TRAITS_HPP_

#include <concepts>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

template<typename Type>
concept entity = std::is_same_v<Type, uint32> || (std::is_enum_v<Type> && std::is_same_v<std::underlying_type_t<Type>, uint32>);

template<entity Entity>
struct entity_traits {
  
  using value_type = Entity;
  using entity_type = uint32;
  using version_type = uint16;

  static constexpr auto entity_mask = entity_type{0xFFFFF};
  static constexpr auto version_mask = entity_type{0xFFF};
  static constexpr auto entity_shift = std::size_t{20};
  
  static constexpr auto reserved = entity_type{entity_mask | (version_mask << entity_shift)};

  [[nodiscard]] static constexpr entity_type to_integral(const value_type value) noexcept;

  [[nodiscard]] static constexpr entity_type to_entity(const value_type value) noexcept;

  [[nodiscard]] static constexpr version_type to_version(const value_type value) noexcept;

  [[nodiscard]] static constexpr value_type construct(const entity_type entity, const version_type version) noexcept;

  [[nodiscard]] static constexpr value_type combine(const entity_type lhs, const entity_type rhs) noexcept;

}; // struct entity_traits

enum class entity_t : uint32 { };

struct null_entity_t {
  
  template<entity Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept;

}; // struct null_entity_t

[[nodiscard]] constexpr bool operator==(const null_entity_t lhs, const null_entity_t rhs) noexcept;

template<entity Entity>
[[nodiscard]] constexpr bool operator==(const null_entity_t lhs, const Entity rhs) noexcept;

inline constexpr null_entity_t null_entity{};

struct tombstone_entity_t {

  template<entity Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept;

}; // struct tombstone_entity_t

[[nodiscard]] constexpr bool operator==(const tombstone_entity_t lhs, const tombstone_entity_t rhs) noexcept;

template<entity Entity>
[[nodiscard]] constexpr bool operator==(const tombstone_entity_t lhs, const Entity rhs) noexcept;

inline constexpr tombstone_entity_t tombstone_entity{};

} // namespace sbx

#include "entity_traits.inl"

#endif // SBX_ECS_ENTITY_TRAITS_HPP_
