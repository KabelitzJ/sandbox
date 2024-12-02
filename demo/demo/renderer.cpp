#include <demo/renderer.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/models/models.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/post/filters/resolve_filter.hpp>
#include <libsbx/post/filters/blur_filter.hpp>
#include <libsbx/post/filters/fxaa_filter.hpp>

#include <libsbx/shadows/shadow_subrenderer.hpp>
#include <libsbx/ui/ui_subrenderer.hpp>
#include <libsbx/gizmos/gizmos_subrenderer.hpp>

#include <demo/terrain/terrain_subrenderer.hpp>
#include <demo/terrain/planet_generator_task.hpp>

#include <demo/imgui/imgui_subrenderer.hpp>

namespace demo {

renderer::renderer()
: _clear_color{sbx::math::color::black} {
  // Render stage 0: Shadow map
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
      sbx::graphics::attachment{1, "shadow_map", sbx::graphics::attachment::type::image, sbx::math::color::white, sbx::graphics::format::r32_sfloat, sbx::graphics::address_mode::clamp_to_edge}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings), sbx::graphics::viewport{sbx::math::vector2u{2048, 2048}});
  }

  // Render stage 1: Deferred scene
  // {
  //   auto attachments = std::vector<sbx::graphics::attachment>{
  //     sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
  //     sbx::graphics::attachment{1, "position", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r32g32b32a32_sfloat},
  //     sbx::graphics::attachment{2, "normal", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r32g32b32a32_sfloat},
  //     sbx::graphics::attachment{3, "albedo", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm},
  //     sbx::graphics::attachment{4, "normalized_depth", sbx::graphics::attachment::type::image, sbx::math::color::white, sbx::graphics::format::r32_sfloat},
  //     sbx::graphics::attachment{5, "resolve", sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm}
  //   };

  //   auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
  //     sbx::graphics::subpass_binding{0, {0, 1, 2, 3, 4}},
  //     sbx::graphics::subpass_binding{1, {5}, {1, 2, 3}}
  //   };

  //   add_render_stage(std::move(attachments), std::move(subpass_bindings));
  // }

  // Render stage 2: FX and UI
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
      sbx::graphics::attachment{1, "swapchain", sbx::graphics::attachment::type::swapchain, _clear_color, sbx::graphics::format::r8g8b8a8_unorm}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1}},
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }
}

auto renderer::initialize() -> void {
  // Task 0
  // auto& planet_generator_task = add_task<demo::planet_generator_task>("demo/assets/shaders/planet_generator");

  // Render stage 0
  add_subrenderer<sbx::shadows::shadow_subrenderer>("demo/assets/shaders/shadow", sbx::graphics::pipeline::stage{0, 0});

  // Render stage 1
  // add_subrenderer<demo::terrain_subrenderer>("demo/assets/shaders/terrain", sbx::graphics::pipeline::stage{1, 0});
  add_subrenderer<sbx::models::static_mesh_subrenderer>("demo/assets/shaders/static_mesh", sbx::graphics::pipeline::stage{1, 0});

  // auto attachment_names = std::unordered_map<std::string, std::string>{
  //   {"position_image", "position"},
  //   {"normal_image", "normal"},
  //   {"albedo_image", "albedo"},
  //   {"shadow_map_image", "shadow_map"}
  // };

  // add_subrenderer<sbx::post::resolve_filter<sbx::graphics::empty_vertex>>("demo/assets/shaders/resolve", sbx::graphics::pipeline::stage{1, 1}, std::move(attachment_names));

  // Render stage 2
  // add_subrenderer<sbx::post::fxaa_filter<sbx::graphics::empty_vertex>>("demo/assets/shaders/fxaa", sbx::graphics::pipeline::stage{2, 0}, "resolve");
  // add_subrenderer<sbx::gizmos::gizmos_subrenderer>("demo/assets/shaders/gizmos", sbx::graphics::pipeline::stage{2, 0}, "normalized_depth");
  add_subrenderer<sbx::ui::ui_subrenderer>("demo/assets/shaders/ui", sbx::graphics::pipeline::stage{1, 0});

  add_subrenderer<demo::imgui_subrenderer>("demo/assets/shaders/imgui", sbx::graphics::pipeline::stage{1, 0});
}

} // namespace demo
