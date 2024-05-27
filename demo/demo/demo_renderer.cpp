#include <demo/demo_renderer.hpp>

#include <libsbx/core/engine.hpp>
#include <libsbx/models/models.hpp>

namespace demo {

demo_renderer::demo_renderer()
: _clear_color{0.52f, 0.80f, 0.98f, 1.0f} {
  // Render stage 0: Deferred scene
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
      sbx::graphics::attachment{1, "position", sbx::graphics::attachment::type::image, sbx::graphics::format::r32g32b32a32_sfloat, _clear_color},
      sbx::graphics::attachment{2, "normal", sbx::graphics::attachment::type::image, sbx::graphics::format::r32g32b32a32_sfloat, _clear_color},
      sbx::graphics::attachment{3, "albedo", sbx::graphics::attachment::type::image, sbx::graphics::format::r8g8b8a8_unorm, _clear_color},
      sbx::graphics::attachment{4, "swapchain", sbx::graphics::attachment::type::swapchain, sbx::graphics::format::r8g8b8a8_unorm, _clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      // sbx::graphics::subpass_binding{0, {0, 1, 2, 3}},
      sbx::graphics::subpass_binding{0, {0, 1, 2, 3, 4}},
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }
}

auto demo_renderer::initialize() -> void {
  auto& cli  = sbx::core::engine::cli();

  // Render stage 0
  add_subrenderer<sbx::models::mesh_subrenderer>("demo/assets/shaders/deferred", sbx::graphics::pipeline::stage{0, 0});
}

} // namespace demo
