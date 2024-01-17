#include <demo/demo_renderer.hpp>

#include <libsbx/models/models.hpp>
#include <libsbx/ui/ui.hpp>
#include <libsbx/shadows/shadows.hpp>
#include <libsbx/post/post.hpp>

namespace demo {

demo_renderer::demo_renderer() {
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "shadow_map", sbx::graphics::attachment::type::image, VK_FORMAT_R32G32_SFLOAT, sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f}}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings), sbx::graphics::viewport{sbx::math::vector2u{4096, 4096}});
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

  add_subrenderer<sbx::models::mesh_subrenderer>("res://shaders/mesh", sbx::graphics::pipeline::stage{1, 0});
  add_subrenderer<sbx::ui::ui_subrenderer>("res://shaders/ui", sbx::graphics::pipeline::stage{1, 1});

  // add_subrenderer<sbx::post::blur_filter<sbx::models::vertex3d>>("res://shaders/blur", sbx::graphics::pipeline::stage{1, 2}, sbx::math::vector2{0.5f, 0.5f});
}

} // namespace demo
