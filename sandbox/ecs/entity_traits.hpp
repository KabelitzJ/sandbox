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

  [[nodiscard]] static constexpr entity_type to_underlying(const value_type value) noexcept;

}; // struct entity_traits

enum class entity_id : uint32 { };

} // namespace sbx

#include "entity_traits.inl"

#endif // SBX_ECS_ENTITY_TRAITS_HPP_
