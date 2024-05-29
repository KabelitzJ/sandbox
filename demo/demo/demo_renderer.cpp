#include <demo/demo_renderer.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/models/models.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/post/filters/resolve_filter.hpp>

#include <libsbx/ui/ui_subrenderer.hpp>

namespace demo {

demo_renderer::demo_renderer()
: _clear_color{0.0f, 0.0f, 0.0f, 1.0f} {
  // Render stage 0: Deferred scene
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
      sbx::graphics::attachment{1, "position", sbx::graphics::attachment::type::image, sbx::graphics::format::r32g32b32a32_sfloat, _clear_color},
      sbx::graphics::attachment{2, "normal", sbx::graphics::attachment::type::image, sbx::graphics::format::r32g32b32a32_sfloat, _clear_color},
      sbx::graphics::attachment{3, "albedo", sbx::graphics::attachment::type::image, sbx::graphics::format::r8g8b8a8_unorm, _clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1, 2, 3}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }

  // Render stage 1: Deferred resolve
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, sbx::graphics::format::r8g8b8a8_unorm, _clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}},
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }
}

auto demo_renderer::initialize() -> void {
  // Render stage 0
  add_subrenderer<sbx::models::mesh_subrenderer>("demo/assets/shaders/deferred", sbx::graphics::pipeline::stage{0, 0});

  auto attachment_names = std::unordered_map<std::string, std::string>{
    {"position_image", "position"},
    {"normal_image", "normal"},
    {"albedo_image", "albedo"}
  };

  // Render stage 1
  add_subrenderer<sbx::post::resolve_filter<sbx::graphics::empty_vertex>>("demo/assets/shaders/resolve", sbx::graphics::pipeline::stage{1, 0}, std::move(attachment_names));
  add_subrenderer<sbx::ui::ui_subrenderer>("demo/assets/shaders/ui", sbx::graphics::pipeline::stage{1, 0});
}

} // namespace demo
