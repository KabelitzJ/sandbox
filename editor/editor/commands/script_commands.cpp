// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <editor/commands/script_commands.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scripting/scripting_module.hpp>

namespace editor {

auto attach_script_command::execute(sbx::scenes::scene& target) -> void {
  if (auto node = target.find(_node_id); node.is_valid()) {
    sbx::core::engine::get_module<sbx::scripting::scripting_module>().attach_script(node, _class_name);
  }
}

auto attach_script_command::undo(sbx::scenes::scene& target) -> void {
  if (auto node = target.find(_node_id); node.is_valid()) {
    sbx::core::engine::get_module<sbx::scripting::scripting_module>().detach_script(node, _class_name);
  }
}

auto detach_script_command::execute(sbx::scenes::scene& target) -> void {
  if (auto node = target.find(_node_id); node.is_valid()) {
    sbx::core::engine::get_module<sbx::scripting::scripting_module>().detach_script(node, _before.class_name);
  }
}

auto detach_script_command::undo(sbx::scenes::scene& target) -> void {
  auto node = target.find(_node_id);

  if (!node.is_valid()) {
    return;
  }

  // Reattach the snapshotted entry directly (not via attach_script(), which would seed a fresh,
  // override-less one) — this restores the field overrides that existed before detach.
  auto& scripts = node.get_or_add_component<sbx::scenes::script_component>();
  scripts.scripts.push_back(_before);

  auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

  if (scenes_module.is_simulating()) {
    sbx::core::engine::get_module<sbx::scripting::scripting_module>().instantiate(node, _before.class_name);
  }
}

} // namespace editor
