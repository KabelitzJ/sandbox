#include <demo/demo_renderer.hpp>

#include <libsbx/models/models.hpp>
#include <libsbx/ui/ui.hpp>
#include <libsbx/shadows/shadows.hpp>
#include <libsbx/post/post.hpp>

namespace demo {

demo_renderer::demo_renderer()
: _sky_color{0.52f, 0.80f, 0.98f, 1.0f} {
  // Render stage 0: Shadow map
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "shadow_map", sbx::graphics::attachment::type::image, VK_FORMAT_R32G32_SFLOAT, sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f}}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings), sbx::graphics::viewport{sbx::math::vector2u{4096, 4096}});
  }

  // Render stage 1: Scene
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
      sbx::graphics::attachment{1, "position", sbx::graphics::attachment::type::image, VK_FORMAT_R32G32B32A32_SFLOAT, _sky_color},
      sbx::graphics::attachment{2, "normal", sbx::graphics::attachment::type::image, VK_FORMAT_R32G32B32A32_SFLOAT, _sky_color},
      sbx::graphics::attachment{3, "albedo", sbx::graphics::attachment::type::image, VK_FORMAT_R8G8B8A8_UNORM, _sky_color},
      sbx::graphics::attachment{4, "resolve", sbx::graphics::attachment::type::image, VK_FORMAT_R8G8B8A8_UNORM, _sky_color},
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1, 2, 3}},
      sbx::graphics::subpass_binding{1, {0, 4}}
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }

  // Render stage 2: Deferred resolve and UI
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, VK_FORMAT_R8G8B8A8_UNORM}
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
  add_subrenderer<sbx::models::mesh_subrenderer>("res://shaders/mesh", sbx::graphics::pipeline::stage{1, 0});

  add_subrenderer<sbx::post::resolve_filter<sbx::graphics::empty_vertex>>("res://shaders/resolve", sbx::graphics::pipeline::stage{2, 0}, "albedo");
  add_subrenderer<sbx::ui::ui_subrenderer>("res://shaders/ui", sbx::graphics::pipeline::stage{2, 0});

}

} // namespace demo
