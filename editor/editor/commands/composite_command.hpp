// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef EDITOR_COMMANDS_COMPOSITE_COMMAND_HPP_
#define EDITOR_COMMANDS_COMPOSITE_COMMAND_HPP_

#include <memory>
#include <string>
#include <vector>

#include <libsbx/scenes/scene.hpp>

#include <editor/commands/command.hpp>

namespace editor {

/**
 * @brief Wraps several commands into one undo/redo step.
 *
 * execute() runs every sub-command forward, in order; undo() runs them in reverse. Every
 * sub-command must already be fully constructed (its own "before" state captured) before this is
 * built, since command_stack::push() calls execute() exactly once, on the composite as a whole.
 *
 * Used wherever a single user gesture affects several nodes at once (multi-select drag/reparent,
 * multi-select delete, group-gizmo transform) — a lone sub-command is never wrapped here, callers
 * push it directly instead, to keep single-target Undo labels/shape unchanged.
 */
class composite_command final : public command {

public:

  composite_command(std::vector<std::unique_ptr<command>> commands, std::string label);

  auto execute(sbx::scenes::scene& target) -> void override;

  auto undo(sbx::scenes::scene& target) -> void override;

  [[nodiscard]] auto label() const -> std::string override {
    return _label;
  }

private:

  std::vector<std::unique_ptr<command>> _commands;
  std::string _label;

}; // class composite_command

} // namespace editor

#endif // EDITOR_COMMANDS_COMPOSITE_COMMAND_HPP_
