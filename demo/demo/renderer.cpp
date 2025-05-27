#include <demo/renderer.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/models/models.hpp>

#include <libsbx/scenes/skybox_subrenderer.hpp>
#include <libsbx/scenes/debug_subrenderer.hpp>
#include <libsbx/scenes/grid_subrenderer.hpp>
#include <libsbx/scenes/frustum_culling_task.hpp>

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
      sbx::graphics::attachment{5, "normalized_depth", sbx::graphics::attachment::type::image, sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f}, sbx::graphics::format::r32_sfloat},
      sbx::graphics::attachment{6, "scene", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1, 2, 3, 4, 5}},
      sbx::graphics::subpass_binding{1, {6}, {1, 2, 3, 4}}
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
  auto& logical_device = graphics_module.logical_device();

  // Compute stage

  auto& frustum_culling_task = add_task<sbx::scenes::frustum_culling_task>("demo/assets/shaders/frustum_culling");

  auto& draw_commands_buffer = graphics_module.get_resource<sbx::graphics::buffer>(frustum_culling_task.draw_commands_buffer());

  // Compute -> Graphics ownership transfer

  graphics_module.release_ownership({
    .src_stage_mask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
    .src_access_mask = VK_ACCESS_2_SHADER_WRITE_BIT,
    .src_queue_family = logical_device.queue<sbx::graphics::queue::type::compute>().family(),
    .dst_queue_family = logical_device.queue<sbx::graphics::queue::type::graphics>().family(),
    .buffer = draw_commands_buffer
  });

  graphics_module.acquire_ownership({
    .dst_stage_mask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
    .dst_access_mask = VK_ACCESS_2_SHADER_READ_BIT,
    .src_queue_family = logical_device.queue<sbx::graphics::queue::type::graphics>().family(),
    .dst_queue_family = logical_device.queue<sbx::graphics::queue::type::compute>().family(),
    .buffer = draw_commands_buffer
  });

  // Render stage 0
  // add_subrenderer<sbx::shadows::shadow_subrenderer>("demo/assets/shaders/shadow", sbx::graphics::pipeline::stage{0, 0});

  // Render stage 1
  add_subrenderer<sbx::scenes::skybox_subrenderer>("demo/assets/shaders/skybox", sbx::graphics::pipeline::stage{0, 0});
  add_subrenderer<sbx::scenes::grid_subrenderer>("demo/assets/shaders/grid", sbx::graphics::pipeline::stage{0, 0});
  add_subrenderer<sbx::models::static_mesh_subrenderer>("demo/assets/shaders/deferred", sbx::graphics::pipeline::stage{0, 0}, frustum_culling_task.draw_commands_buffer());
  add_subrenderer<sbx::scenes::debug_subrenderer>("demo/assets/shaders/debug", sbx::graphics::pipeline::stage{0, 0});
  
  // add_subrenderer<sbx::models::static_mesh_subrenderer>("demo/assets/shaders/static_mesh", sbx::graphics::pipeline::stage{0, 0});

  auto attachment_names = std::unordered_map<std::string, std::string>{
    {"albedo_image", "albedo"},
    {"position_image", "position"},
    {"normal_image", "normal"},
    {"material_image", "material"}
  };

  add_subrenderer<sbx::post::resolve_filter>("demo/assets/shaders/resolve", sbx::graphics::pipeline::stage{0, 1}, std::move(attachment_names));

  // // Render stage 2
  add_subrenderer<sbx::editor::editor_subrenderer>("demo/assets/shaders/editor", sbx::graphics::pipeline::stage{1, 0});
}

} // namespace demo
