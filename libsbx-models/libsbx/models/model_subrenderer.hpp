#ifndef LIBSBX_MODELS_MODEL_SUBRENDERER_HPP_
#define LIBSBX_MODELS_MODEL_SUBRENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/assets/assets_module.hpp>

namespace sbx::models {

class model_subrenderer : public graphics::subrenderer {

public:

  model_subrenderer(const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage} { }

  ~model_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& scene_module = scenes::scenes_module::get();
    auto& asset_module = assets::assets_module::get();

    auto& scene = scene_module.scene();

    auto node = scene.create_node();

    node.add_component<mesh::handle_type>(asset_module.load_asset<mesh>("assets/models/monkey.obj"));

    auto mesh_nodes = scene.query<mesh::handle_type>();

    for (auto& node : mesh_nodes) {
      auto mesh_handle = node.get_component<mesh::handle_type>();

      auto& mesh_instance = asset_module.get_asset<mesh>(*mesh_handle);
    }
  }

private:

}; // class model_subrenderer

} // namespace sbx::models

#endif // LIBSBX_MODELS_MODEL_SUBRENDERER_HPP_
