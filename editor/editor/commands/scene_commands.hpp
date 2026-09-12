// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef EDITOR_COMMANDS_SCENE_COMMANDS_HPP_
#define EDITOR_COMMANDS_SCENE_COMMANDS_HPP_

#include <cstddef>
#include <optional>
#include <string>

#include <yaml-cpp/yaml.h>

#include <libsbx/math/uuid.hpp>

#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/scene.hpp>

#include <libsbx/assets/primitive_meshes.hpp>
#include <libsbx/assets/prefab.hpp>

#include <editor/commands/command.hpp>

namespace editor {

/** @brief Creates one new node (no children), optionally parented under parent_id. */
class create_node_command final : public command {

public:

  explicit create_node_command(std::optional<sbx::math::uuid> parent_id = std::nullopt, std::string name = "Node");

  auto execute(sbx::scenes::scene& target) -> void override;

  auto undo(sbx::scenes::scene& target) -> void override;

  [[nodiscard]] auto label() const -> std::string override {
    return "Create Node";
  }

  /** @brief The created node's id — valid to read right after command_stack::push() returns. */
  [[nodiscard]] auto id() const noexcept -> sbx::math::uuid {
    return _id;
  }

private:

  std::optional<sbx::math::uuid> _parent_id;
  std::string _name;
  sbx::math::uuid _id{sbx::math::uuid::nil()};

}; // class create_node_command

/** @brief Creates one new node with a mesh_renderer already pointed at a built-in primitive mesh — the Hierarchy panel's "Create > 3D Object" menu. */
class create_primitive_node_command final : public command {

public:

  explicit create_primitive_node_command(sbx::assets::primitive_mesh_kind kind, std::optional<sbx::math::uuid> parent_id = std::nullopt);

  auto execute(sbx::scenes::scene& target) -> void override;

  auto undo(sbx::scenes::scene& target) -> void override;

  [[nodiscard]] auto label() const -> std::string override {
    return "Create " + std::string{sbx::assets::primitive_mesh_name(_kind)};
  }

  /** @brief The created node's id — valid to read right after command_stack::push() returns. */
  [[nodiscard]] auto id() const noexcept -> sbx::math::uuid {
    return _id;
  }

private:

  sbx::assets::primitive_mesh_kind _kind;
  std::optional<sbx::math::uuid> _parent_id;
  sbx::math::uuid _id{sbx::math::uuid::nil()};

}; // class create_primitive_node_command

/** @brief Instantiates prefab as a new subtree, optionally parented under parent_id — the Hierarchy panel's prefab drag-drop. */
class instantiate_prefab_command final : public command {

public:

  explicit instantiate_prefab_command(sbx::assets::prefab_handle prefab, std::optional<sbx::math::uuid> parent_id = std::nullopt);

  auto execute(sbx::scenes::scene& target) -> void override;

  auto undo(sbx::scenes::scene& target) -> void override;

  [[nodiscard]] auto label() const -> std::string override {
    return "Instantiate " + (_prefab.is_valid() ? _prefab->name() : std::string{"Prefab"});
  }

  /** @brief The created instance's root id — valid to read right after command_stack::push() returns. */
  [[nodiscard]] auto id() const noexcept -> sbx::math::uuid {
    return _id;
  }

private:

  sbx::assets::prefab_handle _prefab;
  std::optional<sbx::math::uuid> _parent_id;
  sbx::math::uuid _id{sbx::math::uuid::nil()};

}; // class instantiate_prefab_command

/**
 * @brief Deletes target and its whole subtree. Snapshots everything undo needs to restore it —
 * components, structure, ids, sibling position, and any active-camera/primary-light binding — at
 * construction time, before anything is actually deleted.
 */
class delete_node_command final : public command {

public:

  explicit delete_node_command(sbx::scenes::scene& scene, const sbx::scenes::node& target);

  auto execute(sbx::scenes::scene& target) -> void override;

  auto undo(sbx::scenes::scene& target) -> void override;

  [[nodiscard]] auto label() const -> std::string override {
    return "Delete Node";
  }

private:

  sbx::math::uuid _id;
  std::optional<sbx::math::uuid> _parent_id{}; // nullopt = was top-level
  std::size_t _index{0u};
  YAML::Node _snapshot;
  std::optional<sbx::math::uuid> _was_active_camera{};
  std::optional<sbx::math::uuid> _was_primary_light{};

}; // class delete_node_command

/**
 * @brief Moves target to a new parent (nullopt = top-level) at new_index among that parent's
 * children, restoring its original parent/index on undo. Backs the Hierarchy panel's drag/drop
 * reparent and reorder.
 *
 * new_index is relative to the destination list *before* target is removed from wherever it
 * currently sits — execute() corrects for the shift itself when target is moving within the same
 * parent, so callers just pass the raw drop-target position.
 */
class reparent_node_command final : public command {

public:

  explicit reparent_node_command(sbx::scenes::scene& scene, const sbx::scenes::node& target, std::optional<sbx::math::uuid> new_parent_id, std::size_t new_index);

  auto execute(sbx::scenes::scene& target) -> void override;

  auto undo(sbx::scenes::scene& target) -> void override;

  [[nodiscard]] auto label() const -> std::string override {
    return "Move Node";
  }

private:

  sbx::math::uuid _id;
  std::optional<sbx::math::uuid> _old_parent_id{}; // nullopt = was top-level
  std::size_t _old_index{0u};
  std::optional<sbx::math::uuid> _new_parent_id;
  std::size_t _new_index;

}; // class reparent_node_command

/** @brief Sets the scene's active (play) camera to target, restoring whatever it was before on undo. */
class set_active_camera_command final : public command {

public:

  explicit set_active_camera_command(sbx::scenes::scene& scene, const sbx::scenes::node& target);

  auto execute(sbx::scenes::scene& target) -> void override;

  auto undo(sbx::scenes::scene& target) -> void override;

  [[nodiscard]] auto label() const -> std::string override {
    return "Set Active Camera";
  }

private:

  sbx::math::uuid _id;
  std::optional<sbx::math::uuid> _previous_id{}; // nullopt = there was no active camera before

}; // class set_active_camera_command

} // namespace editor

#endif // EDITOR_COMMANDS_SCENE_COMMANDS_HPP_
