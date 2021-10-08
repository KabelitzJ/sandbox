#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

template<typename, typename = void>
struct entity_traits;

template<typename Type>
struct entity_traits<Type, std::enable_if_t<std::is_same_v<Type, uint32>>> : entity_traits<Type> { };

template<typename Type>
struct entity_traits<Type, std::enable_if_t<std::is_enum_v<Type>>> : entity_traits<std::underlying_type_t<Type>> { };

template<>
struct entity_traits<uint32> {
  using value_type = uint32;

  using entity_type = uint32;
  using version_type = uint16;

  static constexpr entity_type entity_mask = 0xFFFFF;
  static constexpr entity_type version_mask = 0xFFF;
  static constexpr std::size_t entity_shift = 20u;

  static constexpr entity_type reserved = entity_mask | (version_mask << entity_shift);

  [[nodiscard]] static constexpr entity_type to_integral(const )
};

enum class entity : uint32 { };

struct null_entity_t {

}; // struct null_entity_t

inline constexpr null_entity_t null_entity{};

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
