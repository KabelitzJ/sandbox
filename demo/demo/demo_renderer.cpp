#include <demo/demo_renderer.hpp>

#include <libsbx/core/engine.hpp>
#include <libsbx/models/models.hpp>
#include <libsbx/ui/ui.hpp>
#include <libsbx/shadows/shadows.hpp>
#include <libsbx/post/post.hpp>

namespace demo {

demo_renderer::demo_renderer()
: _clear_color{0.52f, 0.80f, 0.98f, 1.0f},
  _shadow_map_clear_color{1.0f, 1.0f, 1.0f, 1.0f},
  _shadow_map_size{2048, 2048},
  _shadow_map_format{sbx::graphics::format::r32g32_sfloat} {
  // Render stage 0: Shadow map
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "shadow_map", sbx::graphics::attachment::type::image, _shadow_map_format, _shadow_map_clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings), sbx::graphics::viewport{_shadow_map_size});
  }

  // Render stage 1: Shadow map blur
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "blurred_shadow_map", sbx::graphics::attachment::type::image, _shadow_map_format, _shadow_map_clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings), sbx::graphics::viewport{_shadow_map_size});
  }

  // Render stage 2: Deferred scene
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
      sbx::graphics::attachment{1, "position", sbx::graphics::attachment::type::image, sbx::graphics::format::r32g32b32a32_sfloat, _clear_color},
      sbx::graphics::attachment{2, "normal", sbx::graphics::attachment::type::image, sbx::graphics::format::r32g32b32a32_sfloat, _clear_color},
      sbx::graphics::attachment{3, "color", sbx::graphics::attachment::type::image, sbx::graphics::format::r8g8b8a8_unorm, _clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1, 2, 3}},
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }

  // Render stage 3: Deferred resolve and UI
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, sbx::graphics::format::r8g8b8a8_unorm, _clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }
}

auto demo_renderer::initialize() -> void {
  // Render stage 0
  add_subrenderer<sbx::shadows::shadow_subrenderer>("res://shaders/shadow", sbx::graphics::pipeline::stage{0, 0});

  // Render stage 1
  add_subrenderer<sbx::post::blur_filter<sbx::graphics::empty_vertex>>("res://shaders/blur", sbx::graphics::pipeline::stage{1, 0}, "shadow_map", sbx::math::vector2{0.5f, 0.5f});

  // Render stage 2
  add_subrenderer<sbx::models::mesh_subrenderer>("res://shaders/mesh", sbx::graphics::pipeline::stage{2, 0});

  auto attachment_names = std::unordered_map<std::string, std::string>{
    {"position", "position"},
    {"normal", "normal"},
    {"color", "color"},
  };

  // Render stage 3
  add_subrenderer<sbx::post::resolve_filter<sbx::graphics::empty_vertex>>("res://shaders/resolve", sbx::graphics::pipeline::stage{3, 0}, std::move(attachment_names));
  add_subrenderer<sbx::ui::ui_subrenderer>("res://shaders/ui", sbx::graphics::pipeline::stage{3, 0});

}

} // namespace demo
