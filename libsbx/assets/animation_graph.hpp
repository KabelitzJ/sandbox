// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_ASSETS_ANIMATION_GRAPH_HPP_
#define LIBSBX_ASSETS_ANIMATION_GRAPH_HPP_

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <libsbx/math/uuid.hpp>
#include <libsbx/math/vector2.hpp>
#include <libsbx/math/bounded.hpp>

#include <libsbx/assets/asset_handle.hpp>
#include <libsbx/assets/loadable.hpp>

namespace sbx::assets {

struct animation_trigger {
  bool set{false};
}; // struct animation_trigger

using animation_parameter_value = std::variant<std::float_t, bool, std::int32_t, animation_trigger>;

struct animation_parameter {
  std::string name{};
  animation_parameter_value default_value{0.0f};
}; // struct animation_parameter

struct animation_state {
  std::uint32_t id{0u};
  std::string name{};
  std::string clip_name{};
  std::float_t speed{1.0f};
  bool loop{true};
  math::vector2 editor_position{0.0f, 0.0f};
}; // struct animation_state

enum class animation_condition_comparator : std::uint8_t {
  equals,
  not_equals,
  greater,
  greater_or_equal,
  less,
  less_or_equal
}; // enum class animation_condition_comparator

struct animation_condition {
  std::string parameter_name{};
  animation_condition_comparator comparator{animation_condition_comparator::equals};
  animation_parameter_value expected{0.0f};
}; // struct animation_condition

struct animation_transition {
  std::optional<std::uint32_t> from_state{};
  std::uint32_t to_state{0u};
  std::float_t duration{0.25f};
  bool has_exit_time{false};
  math::bounded<std::float_t, 0.0f, 1.0f> exit_time{1.0f};
  std::vector<animation_condition> conditions{};
}; // struct animation_transition

class animation_graph final : public loadable {

  friend class asset_residency;

public:

  struct create_info {
    std::string name{"animation_graph"};
    std::vector<animation_parameter> parameters{};
    std::vector<animation_state> states{};
    std::vector<animation_transition> transitions{};
    std::uint32_t entry_state_id{0u};
  }; // struct create_info

  animation_graph() = default;

  explicit animation_graph(const create_info& create_info)
  : _name{create_info.name},
    _parameters{create_info.parameters},
    _states{create_info.states},
    _transitions{create_info.transitions},
    _entry_state_id{create_info.entry_state_id} { }

  [[nodiscard]] auto is_valid() const noexcept -> bool {
    return !_states.empty();
  }

  [[nodiscard]] auto id() const noexcept -> const math::uuid& {
    return _id;
  }

  [[nodiscard]] auto name() const noexcept -> const std::string& {
    return _name;
  }

  [[nodiscard]] auto parameters() const noexcept -> const std::vector<animation_parameter>& {
    return _parameters;
  }

  [[nodiscard]] auto states() const noexcept -> const std::vector<animation_state>& {
    return _states;
  }

  [[nodiscard]] auto transitions() const noexcept -> const std::vector<animation_transition>& {
    return _transitions;
  }

  [[nodiscard]] auto entry_state_id() const noexcept -> std::uint32_t {
    return _entry_state_id;
  }

  [[nodiscard]] auto next_state_id() const noexcept -> std::uint32_t {
    auto next = std::uint32_t{0u};

    for (const auto& state : _states) {
      next = std::max(next, state.id + 1u);
    }

    return next;
  }

private:

  std::string _name{"animation_graph"};
  std::vector<animation_parameter> _parameters{};
  std::vector<animation_state> _states{};
  std::vector<animation_transition> _transitions{};
  std::uint32_t _entry_state_id{0u};
  math::uuid _id{math::uuid::nil()};

}; // class animation_graph

using animation_graph_handle = asset_handle<animation_graph>;

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ANIMATION_GRAPH_HPP_
