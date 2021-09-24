#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

// (TODO) KAJ 22.09.2021 19.01 Find concept for entity representation
// class entity {

// public:
//   explicit entity(uint32 id);
//   entity(uint16 index, uint16 version);
//   ~entity() = default;

//   uint32 id() const;
//   uint16 index() const;
//   uint16 version() const;

//   operator uint32() const;

// private:
//   uint32 _id;

// }; // class entity

template<typename, typename = void>
struct is_entity : std::false_type {};

template<typename Type>
struct is_entity<
  Type, 
  std::enable_if_t<std::is_same_v<Type, uint32>>    
> : std::true_type {};

template <typename Type>
struct is_entity<
  Type,
  std::enable_if_t<std::is_enum_v<Type>>
> : is_entity<std::underlying_type_t<Type>> {};

template<typename Type>
inline constexpr auto is_entity_v = is_entity<Type>::value;

template<typename, typename = void>
struct entity_traits;

template<typename Type>
struct entity_traits<Type, std::enable_if_t<is_entity_v<Type>>> {

  using value_type = Type;

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
[[nodiscard]] constexpr auto to_integral(const Entity entity) noexcept {
  return entity_traits<Entity>::to_integral(entity);
}

struct null_entity_t {

  template<typename Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept {
    using traits = entity_traits<Entity>;
    return traits::combine(traits::reserved, traits::reserved);
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

};

template<typename Entity>
[[nodiscard]] constexpr bool operator==(const Entity entity, const null_entity_t other) noexcept {
  return other.operator==(entity);
}

template<typename Entity>
[[nodiscard]] constexpr bool operator!=(const Entity entity, const null_entity_t other) noexcept {
  return !(other == entity);
}

inline constexpr null_entity_t null_entity{};

enum class entity : uint32 {};

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
