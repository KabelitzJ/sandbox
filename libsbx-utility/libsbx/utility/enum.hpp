#ifndef LIBSBX_UTILITY_ENUM_HPP_
#define LIBSBX_UTILITY_ENUM_HPP_

#include <type_traits>
#include <array>
#include <ranges>
#include <optional>

#include <libsbx/utility/string_literal.hpp>

namespace sbx::utility {

template<typename Enum>
requires (std::is_enum_v<Enum>)
constexpr auto to_underlying(const Enum value) -> std::underlying_type_t<Enum> {
  return static_cast<std::underlying_type_t<Enum>>(value);
}

template<typename Enum>
requires (std::is_enum_v<Enum>)
constexpr auto from_underlying(const std::underlying_type_t<Enum> value) -> Enum {
  return static_cast<Enum>(value);
}

template<typename Enum>
requires (std::is_enum_v<Enum>)
struct entry {
  Enum value;
  const char* name;
}; // struct entry

template<typename Enum>
requires (std::is_enum_v<Enum>)
struct enum_mapping;


template<typename Type>
concept mapped_enum = requires() {
  std::is_enum_v<Type>;
  { enum_mapping<Type>::values };
}; // concept mapped_enum

template<mapped_enum Enum>
constexpr auto to_string(const Enum value) -> std::string {
  auto entry = std::ranges::find_if(enum_mapping<Enum>::values, [&value](const auto& entry){ return entry.value == value; });

  if (entry == std::ranges::end(enum_mapping<Enum>::values)) {
    return "<unknown>";
  }

  return entry->name;
}

template<mapped_enum Enum>
constexpr auto from_string(const std::string& string) -> std::optional<Enum> {
  auto entry = std::ranges::find_if(enum_mapping<Enum>::values, [&string](const auto& entry){ return entry.name == string; });

  if (entry == std::ranges::end(enum_mapping<Enum>::values)) {
    return std::nullopt;
  }

  return entry->value;
}

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_ENUM_HPP_
