#include <libsbx/graphics/render_stage.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

auto render_stage::update() -> void {
  auto last_render_area = _render_area;

  _render_area.set_offset(_viewport.offset());

  // [TODO] KAJ 2023-06-14 : This looks a bit scuffed. Maybe try to understand what it actually does and rewrite it.

  if (auto size = _viewport.size(); size) {
    _render_area.set_extent(math::vector2u{_viewport.scale() * (*size)});
  } else {
    auto& window = devices::devices_module::get().window();
    auto window_size = math::vector2u{window.width(), window.height()};

    _render_area.set_extent(math::vector2u{_viewport.scale() * window_size});
  }

  _render_area.set_aspect_ratio(static_cast<std::float_t>(_render_area.extent().x) / static_cast<std::float_t>(_render_area.extent().y));
  _render_area.set_extent(_render_area.extent() + _render_area.offset());

  _is_outdated = last_render_area != _render_area;
}

auto render_stage::rebuild(const swapchain& swapchain) -> void {
  update();

  // [TODO] KAJ 2023-06-14 : Implement this.

  auto& physical_device = graphics_module::get().physical_device();
  auto& logical_device = graphics_module::get().logical_device();
  auto& surface = graphics_module::get().surface();

  if (!_render_pass) {
    _render_pass = std::make_unique<graphics::render_pass>(physical_device, logical_device, surface);
  }
}

} // namespace sbx::graphics
