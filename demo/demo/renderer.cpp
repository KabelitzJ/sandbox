#include <demo/renderer.hpp>

#include <libsbx/core/engine.hpp>


#include <libsbx/scenes/skybox_subrenderer.hpp>
#include <libsbx/scenes/debug_subrenderer.hpp>
#include <libsbx/scenes/grid_subrenderer.hpp>

#include <libsbx/models/models.hpp>
#include <libsbx/models/frustum_culling_task.hpp>
#include <libsbx/models/foliage_task.hpp>
#include <libsbx/models/foliage_subrenderer.hpp>

#include <libsbx/animations/skinned_mesh_subrenderer.hpp>

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
      sbx::graphics::attachment{1, "albedo", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm},
      sbx::graphics::attachment{2, "position", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r32g32b32a32_sfloat},
      sbx::graphics::attachment{3, "normal", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r32g32b32a32_sfloat},
      sbx::graphics::attachment{4, "material", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm},
      sbx::graphics::attachment{5, "object_id", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r32g32_uint},
      sbx::graphics::attachment{6, "normalized_depth", sbx::graphics::attachment::type::image, sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f}, sbx::graphics::format::r32_sfloat},
      sbx::graphics::attachment{7, "resolve", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1, 2, 3, 4, 5, 6}},   // Create deferred G-Buffer
      sbx::graphics::subpass_binding{1, {7}, {1, 2, 3, 4, 5}},       // Deferred resolve
      sbx::graphics::subpass_binding{2, {0, 7}}                   // Forward (transparency/foliage)
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }

  // Render stage 2: FX and UI
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, _clear_color, sbx::graphics::format::r8g8b8a8_unorm}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}},
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }
}

auto renderer::initialize() -> void {
  auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

  // Compute stage

  // auto& frustum_culling_task = add_task<sbx::models::frustum_culling_task>("demo/assets/shaders/frustum_culling");
  // auto& foliage_task = add_task<sbx::models::foliage_task>("demo/assets/shaders/foliage");

  // Compute -> Graphics ownership transfer

  // graphics_module.transfer_ownership<sbx::graphics::queue::type::compute, sbx::graphics::queue::type::graphics>(frustum_culling_task.draw_commands_buffer());
  // graphics_module.transfer_ownership<sbx::graphics::queue::type::compute, sbx::graphics::queue::type::graphics>(foliage_task.grass_output_buffer());
  // graphics_module.transfer_ownership<sbx::graphics::queue::type::compute, sbx::graphics::queue::type::graphics>(foliage_task.draw_command_buffer());
    
  // Render stage 0
  // add_subrenderer<sbx::shadows::shadow_subrenderer>("demo/assets/shaders/shadow", sbx::graphics::pipeline::stage{0, 0});

  // Render stage 1
  add_subrenderer<sbx::scenes::skybox_subrenderer>("demo/assets/shaders/skybox", sbx::graphics::pipeline::stage{0, 0});
  add_subrenderer<sbx::scenes::grid_subrenderer>("demo/assets/shaders/grid", sbx::graphics::pipeline::stage{0, 0});
  // add_subrenderer<sbx::models::static_mesh_subrenderer>("demo/assets/shaders/deferred_static", sbx::graphics::pipeline::stage{0, 0});
  add_subrenderer<sbx::animations::skinned_mesh_subrenderer>("demo/assets/shaders/deferred_skinned", sbx::graphics::pipeline::stage{0, 0});

  auto attachment_names = std::unordered_map<std::string, std::string>{
    {"albedo_image", "albedo"},
    {"position_image", "position"},
    {"normal_image", "normal"},
    {"material_image", "material"},
    {"object_id_image", "object_id"}
  };

  add_subrenderer<sbx::post::resolve_filter>("demo/assets/shaders/resolve", sbx::graphics::pipeline::stage{0, 1}, std::move(attachment_names));

  add_subrenderer<sbx::scenes::debug_subrenderer>("demo/assets/shaders/debug", sbx::graphics::pipeline::stage{0, 2});
  // add_subrenderer<sbx::models::foliage_subrenderer>("demo/assets/shaders/foliage", sbx::graphics::pipeline::stage{0, 2}, foliage_task.grass_output_buffer(), foliage_task.draw_command_buffer());

  // // Render stage 2
  add_subrenderer<sbx::editor::editor_subrenderer>("demo/assets/shaders/editor", sbx::graphics::pipeline::stage{1, 0}, "resolve");
}

} // namespace demo
