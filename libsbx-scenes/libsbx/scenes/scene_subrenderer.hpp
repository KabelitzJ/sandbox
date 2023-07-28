#ifndef LIBSBX_SCENES_SCENE_RENDERER_HPP_
#define LIBSBX_SCENES_SCENE_RENDERER_HPP_

#include <libsbx/graphics/subrenderer.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/models/model.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/scenes/components/mesh_handle.hpp>

namespace sbx::scenes {

class scene_subrenderer : public graphics::subrenderer {

public:

  scene_subrenderer(const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage} { }

  ~scene_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    
  }

}; // class scene_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_SCENE_RENDERER_HPP_
