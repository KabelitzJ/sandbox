#include <demo/demo_renderer.hpp>

namespace demo {

demo_renderer::demo_renderer() {
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "shadow_map", sbx::graphics::attachment::type::depth}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings), sbx::graphics::viewport{sbx::math::vector2u{2048, 2048}});
  }

  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, VK_FORMAT_R8G8B8A8_UNORM, sbx::math::color{0.52f, 0.80f, 0.98f, 1.0f}},
      sbx::graphics::attachment{1, "depth", sbx::graphics::attachment::type::depth}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1}},
      sbx::graphics::subpass_binding{1, {0}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }
}

auto demo_renderer::initialize() -> void {
  add_subrenderer<sbx::shadows::shadow_subrenderer>("res://shaders/shadow", sbx::graphics::pipeline::stage{0, 0});

  add_subrenderer<sbx::models::mesh_subrenderer>("res://shaders/cell_shading", sbx::graphics::pipeline::stage{1, 0});
  add_subrenderer<sbx::ui::ui_subrenderer>("res://shaders/ui", sbx::graphics::pipeline::stage{1, 1});
}

} // namespace demo
