#ifndef LIBSBX_UTILITY_ENUM_HPP_
#define LIBSBX_UTILITY_ENUM_HPP_

#include <type_traits>

namespace sbx::utility {

template<typename Enum>
requires (std::is_enum_v<Enum>)
constexpr auto to_underlying(Enum value) -> std::underlying_type_t<Enum> {
  return static_cast<std::underlying_type_t<Enum>>(value);
}

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_ENUM_HPP_
