#include <demo/renderer.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/models/models.hpp>

#include <libsbx/scenes/skybox_subrenderer.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/post/filters/resolve_filter.hpp>
#include <libsbx/post/filters/blur_filter.hpp>
#include <libsbx/post/filters/fxaa_filter.hpp>

#include <libsbx/shadows/shadow_subrenderer.hpp>
#include <libsbx/ui/ui_subrenderer.hpp>
#include <libsbx/gizmos/gizmos_subrenderer.hpp>
#include <libsbx/editor/editor_subrenderer.hpp>

#include <demo/terrain/terrain_subrenderer.hpp>
#include <demo/terrain/planet_generator_task.hpp>

#include <demo/debug_subrenderer.hpp>

namespace demo {

renderer::renderer()
: _clear_color{sbx::math::color::black()} {
  // Render stage 0: Shadow map
  // {
  //   auto attachments = std::vector<sbx::graphics::attachment>{
  //     sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
  //     sbx::graphics::attachment{1, "shadow_map", sbx::graphics::attachment::type::image, sbx::math::color::white(), sbx::graphics::format::r32_sfloat, sbx::graphics::address_mode::clamp_to_edge}
  //   };

  //   auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
  //     sbx::graphics::subpass_binding{0, {0, 1}}
  //   };

  //   add_render_stage(std::move(attachments), std::move(subpass_bindings), sbx::graphics::viewport{sbx::math::vector2u{2048, 2048}});
  // }

  // Render stage 1: Scene
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
      sbx::graphics::attachment{1, "scene", sbx::graphics::attachment::type::swapchain, _clear_color, sbx::graphics::format::r8g8b8a8_unorm}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1}},
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }

  // Render stage 2: FX and UI
  // {
  //   auto attachments = std::vector<sbx::graphics::attachment>{
  //     sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, _clear_color, sbx::graphics::format::r8g8b8a8_unorm}
  //   };

  //   auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
  //     sbx::graphics::subpass_binding{0, {0}},
  //   };

  //   add_render_stage(std::move(attachments), std::move(subpass_bindings));
  // }
}

auto renderer::initialize() -> void {
  // Render stage 0
  // add_subrenderer<sbx::shadows::shadow_subrenderer>("demo/assets/shaders/shadow", sbx::graphics::pipeline::stage{0, 0});

  // Render stage 1
  add_subrenderer<sbx::scenes::skybox_subrenderer>("demo/assets/shaders/skybox", sbx::graphics::pipeline::stage{0, 0});
  // add_subrenderer<sbx::models::static_mesh_subrenderer>("demo/assets/shaders/static_mesh", sbx::graphics::pipeline::stage{0, 0});

  // add_subrenderer<demo::debug_subrenderer>("demo/assets/shaders/debug", sbx::graphics::pipeline::stage{1, 0});

  // // Render stage 2
  // add_subrenderer<sbx::editor::editor_subrenderer>("demo/assets/shaders/editor", sbx::graphics::pipeline::stage{2, 0});
}

} // namespace demo
