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
  using namespace sbx::utility::literals;

  auto [deferred, resolve, post, editor] = create_graph(
    [&](sbx::graphics::render_graph::context& context) -> sbx::graphics::render_graph::graphics_pass {
      auto deferred_pass = context.graphics_pass("deferred"_hs);

      deferred_pass.produces("depth"_hs, sbx::graphics::attachment::type::depth);
      deferred_pass.produces("albedo"_hs, sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm);
      deferred_pass.produces("position"_hs, sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r32g32b32a32_sfloat);
      deferred_pass.produces("normal"_hs, sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r32g32b32a32_sfloat);
      deferred_pass.produces("material"_hs, sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm);
      deferred_pass.produces("object_id"_hs, sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r32g32_uint);
      deferred_pass.produces("normalized_depth"_hs, sbx::graphics::attachment::type::image, sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f}, sbx::graphics::format::r32_sfloat);

      return deferred_pass;
    },
    [&](sbx::graphics::render_graph::context& context) -> sbx::graphics::render_graph::graphics_pass {
      auto resolve_pass = context.graphics_pass("resolve"_hs);

      resolve_pass.uses("albedo"_hs, "position"_hs, "normal"_hs, "material"_hs, "object_id"_hs);

      resolve_pass.produces("resolve"_hs, sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm);

      return resolve_pass;
    },
    [&](sbx::graphics::render_graph::context& context) -> sbx::graphics::render_graph::graphics_pass {
      auto post_pass = context.graphics_pass("post"_hs);

      post_pass.uses("resolve"_hs);

      post_pass.produces("post"_hs, sbx::graphics::attachment::type::image, _clear_color, sbx::graphics::format::r8g8b8a8_unorm);

      return post_pass;
    },
    [&](sbx::graphics::render_graph::context& context) -> sbx::graphics::render_graph::graphics_pass {
      auto editor_pass = context.graphics_pass("editor"_hs);

      editor_pass.uses("post"_hs);

      editor_pass.produces("swapchain"_hs, sbx::graphics::attachment::type::swapchain, _clear_color, sbx::graphics::format::b8g8r8a8_srgb);

      return editor_pass;
    }
  );

  add_subrenderer<sbx::scenes::skybox_subrenderer>("demo/assets/shaders/skybox", deferred);
  add_subrenderer<sbx::scenes::grid_subrenderer>("demo/assets/shaders/grid", deferred);
  add_subrenderer<sbx::models::static_mesh_subrenderer>("demo/assets/shaders/deferred_static", deferred);
  add_subrenderer<sbx::animations::skinned_mesh_subrenderer>("demo/assets/shaders/deferred_skinned", deferred);

  auto attachment_names = std::vector<std::pair<std::string, std::string>>{
    {"albedo_image", "albedo"},
    {"position_image", "position"},
    {"normal_image", "normal"},
    {"material_image", "material"},
    {"object_id_image", "object_id"}
  };

  add_subrenderer<sbx::post::resolve_filter>("demo/assets/shaders/resolve", resolve, std::move(attachment_names));

  add_subrenderer<sbx::post::fxaa_filter>("demo/assets/shaders/fxaa", post, "resolve");

  add_subrenderer<sbx::editor::editor_subrenderer>("demo/assets/shaders/editor", editor, "post");
}

} // namespace demo
