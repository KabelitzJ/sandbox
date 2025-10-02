#ifndef LIBSBX_ANIMATIONS_ANIMATIONS_MODULE_HPP_
#define LIBSBX_ANIMATIONS_ANIMATIONS_MODULE_HPP_

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/skinned_mesh.hpp>

#include <libsbx/animations/animator.hpp>
#include <libsbx/animations/animation.hpp>
#include <libsbx/animations/mesh.hpp>

namespace sbx::animations {
  
class animations_module : public core::module<animations_module> {

  inline static const auto is_registered = register_module(stage::post);

public:

  animations_module() { }

  ~animations_module() override {

  }

  auto update() -> void override {
    SBX_SCOPED_TIMER("animations_module::update");

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::delta_time();

    auto animator_query = scene.query<animator>();

    for (auto&& [node, animator] : animator_query.each()) {
      animator.update(delta_time);
    }

    auto mesh_query = scene.query<scenes::skinned_mesh>();

    for (auto&& [node, skinned_mesh] : animator_query.each()) {
      
    }
  }

  auto add_animation(scenes::node node, const math::uuid mesh_id, const math::uuid animation_id, const math::uuid material) -> scenes::skinned_mesh& {
    return add_animation(node, mesh_id, animation_id, std::vector<scenes::skinned_mesh::submesh>{{0, material}});
  }

  auto add_animation(scenes::node node, const math::uuid mesh_id, const math::uuid animation_id, const std::vector<scenes::skinned_mesh::submesh>& submeshes) -> scenes::skinned_mesh& {
    auto& assets_module = core::engine::get_module<assets::assets_module>();
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto& skinned_mesh = scene.add_component<sbx::scenes::skinned_mesh>(node, mesh_id, animation_id, submeshes);

    const auto& mesh = assets_module.get_asset<animations::mesh>(mesh_id);
    const auto& animation = assets_module.get_asset<animations::animation>(animation_id);

    const auto& skeleton = mesh.skeleton();

    const auto& bones = skeleton.bones();

    auto nodes = std::vector<scenes::node>{};
    nodes.reserve(bones.size());

    for (auto i = 0u; i < bones.size(); ++i) {
      const auto parent_id = bones[i].parent_id;
      const auto parent = (parent_id == skeleton::bone::null) ? node : nodes.at(parent_id);

      const auto [position, rotation, scale] = math::decompose(bones[i].local_bind_matrix);

      nodes.push_back(scene.create_child_node(parent, skeleton.name_for_bone(i).str(), scenes::transform{position, rotation, scale}));
    }

    skinned_mesh.set_nodes(nodes);

    return skinned_mesh;
  }

  auto find_skeleton_node(scenes::node node, const utility::hashed_string& name) const -> scenes::node {
    auto& assets_module = core::engine::get_module<assets::assets_module>();
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto& skinned_mesh = scene.get_component<sbx::scenes::skinned_mesh>(node);

    const auto& mesh = assets_module.get_asset<animations::mesh>(skinned_mesh.mesh_id());

    const auto& skeleton = mesh.skeleton();

    if (const auto index = skeleton.bone_index(name); index) {
      return skinned_mesh.find_node(*index);
    }

    return scenes::node::null;
  }

}; // class assets_module

} // namespace sbx::animations

#endif // LIBSBX_ANIMATIONS_ANIMATIONS_MODULE_HPP_