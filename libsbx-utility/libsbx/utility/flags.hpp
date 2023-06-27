#ifndef LIBSBX_UTILITY_FLAGS_HPP_
#define LIBSBX_UTILITY_FLAGS_HPP_

#include <concepts>
#include <type_traits>

namespace sbx::utility {

template<typename Enum>
requires (std::is_enum_v<Enum>)
struct enable_bitmask_operators : std::false_type {};

template<typename Enum>
constexpr auto enable_bitmask_operators_v = enable_bitmask_operators<Enum>::value;

template<typename Enum>
concept bitmask_enum = std::is_enum_v<Enum> && enable_bitmask_operators_v<Enum>;

template<typename Enum>
constexpr auto operator|(Enum lhs, Enum rhs) noexcept -> Enum {
  using underlying_type = std::underlying_type_t<Enum>;
  return static_cast<Enum>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs));
}

template<typename Enum>
constexpr auto operator|=(Enum& lhs, Enum rhs) noexcept -> Enum {
  lhs = lhs | rhs;
  return lhs;
}

template<typename Enum>
constexpr auto operator&(Enum lhs, Enum rhs) noexcept -> Enum {
  using underlying_type = std::underlying_type_t<Enum>;
  return static_cast<Enum>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs));
}

template<typename Enum>
constexpr auto operator&=(Enum& lhs, Enum rhs) noexcept -> Enum {
  lhs = lhs & rhs;
  return lhs;
}

template<typename Enum>
constexpr auto operator^(Enum lhs, Enum rhs) noexcept -> Enum {
  using underlying_type = std::underlying_type_t<Enum>;
  return static_cast<Enum>(static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs));
}

template<typename Enum>
constexpr auto operator^=(Enum& lhs, Enum rhs) noexcept -> Enum {
  lhs = lhs ^ rhs;
  return lhs;
}

template<typename Enum>
constexpr auto operator~(Enum lhs) noexcept -> Enum {
  using underlying_type = std::underlying_type_t<Enum>;
  return static_cast<Enum>(~static_cast<underlying_type>(lhs));
}

template<typename Enum>
constexpr auto operator!(Enum lhs) noexcept -> Enum {
  return ~lhs;
}

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_FLAGS_HPP_
