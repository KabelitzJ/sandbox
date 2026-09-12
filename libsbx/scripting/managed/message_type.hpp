// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_SCRIPTING_MANAGED_MESSAGE_TYPE_HPP_
#define LIBSBX_SCRIPTING_MANAGED_MESSAGE_TYPE_HPP_

#include <functional>
#include <utility>

#include <libsbx/scripting/managed/string.hpp>

namespace sbx::scripting::managed {

enum class message_level {
  info = 1 << 0,
  warning = 1 << 1,
  error = 1 << 2,
  all = info | warning | error
}; // enum class message_level

constexpr message_level operator|(const message_level lhs, const message_level rhs) noexcept {
  return static_cast<message_level>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

constexpr bool operator&(const message_level lhs, const message_level rhs) noexcept {
  return (std::to_underlying(lhs) & std::to_underlying(rhs)) != 0;
}

constexpr message_level operator~(const message_level value) noexcept {
  return static_cast<message_level>(~std::to_underlying(value));
}

constexpr message_level& operator|=(message_level& lhs, const message_level& rhs) noexcept {
  return (lhs = (lhs | rhs));
}

} // namespace sbx::scripting::managed

#endif // LIBSBX_SCRIPTING_MANAGED_MESSAGE_TYPE_HPP_