#ifndef SBX_ECS_ENTITY_TRAITS_HPP_
#define SBX_ECS_ENTITY_TRAITS_HPP_

#include <concepts>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

namespace detail {

template<typename Type>
concept entity_32 =
  std::is_same_v<Type, uint32> ||
  (std::is_enum_v<Type> && std::is_same_v<std::underlying_type_t<Type>, uint32>) ||
  (requires { typename Type::entity_type; } && std::is_same_v<uint32, typename Type::entity_type> && std::is_convertible_v<Type, uint32>);

template<typename Type>
concept entity_64 =
  std::is_same_v<Type, uint64> ||
  (std::is_enum_v<Type> && std::is_same_v<std::underlying_type_t<Type>, uint64>) ||
  (requires { typename Type::entity_type; } && std::is_same_v<uint64, typename Type::entity_type> && std::is_convertible_v<Type, uint64>);

struct entity_traits_32 {

  using entity_type = uint32;
  using version_type = uint16;

  inline static constexpr auto entity_mask = entity_type{0xFFFFF};
  inline static constexpr auto version_mask = version_type{0xFFF};
  inline static constexpr auto entity_shift = std::size_t{20};

};

struct entity_traits_64 {

  using entity_type = uint64;
  using version_type = uint32;

  inline static constexpr auto entity_mask = entity_type{0xFFFFFFFF};
  inline static constexpr auto version_mask = version_type{0xFFFFFFFF};
  inline static constexpr auto entity_shift = std::size_t{32};

};

} // namespace detail

template<typename Type>
concept entity = detail::entity_32<Type> || detail::entity_64<Type>;

template<entity Entity>
class entity_traits {

  using base_traits = std::conditional_t<detail::entity_32<Entity>, detail::entity_traits_32, detail::entity_traits_64>;

public:

  using value_type = Entity;
  using entity_type = typename base_traits::entity_type;
  using version_type = typename base_traits::version_type;

  static constexpr auto entity_mask = base_traits::entity_mask;
  static constexpr auto version_mask = base_traits::version_mask;
  static constexpr auto entity_shift = base_traits::entity_shift;
  static constexpr auto reserved = entity_type{entity_mask | (version_mask << entity_shift)};

  [[nodiscard]] static constexpr entity_type to_integral(const value_type value) noexcept {
    return static_cast<entity_type>(value);
  }

  [[nodiscard]] static constexpr entity_type to_entity(const value_type value) noexcept {
    return (to_integral(value) & entity_mask);
  }

  [[nodiscard]] static constexpr version_type to_version(const value_type value) noexcept {
    return (to_integral(value) >> entity_shift);
  }

  [[nodiscard]] static constexpr value_type construct(const entity_type entity, const version_type version) noexcept {
    return value_type{(entity & entity_mask) | (static_cast<entity_type>(version) << entity_shift)};
  }

  [[nodiscard]] static constexpr value_type combine(const entity_type lhs, const entity_type rhs) noexcept {
    return value_type{(lhs & entity_mask) | (rhs & (version_mask << entity_shift))};
  }

}; // class entity_traits

} // namespace sbx

#endif // SBX_ECS_ENTITY_TRAITS_HPP_
