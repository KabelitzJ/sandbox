#ifndef DEMO_DEMO_RENDERER_HPP_
#define DEMO_DEMO_RENDERER_HPP_

#include <libsbx/graphics/graphics.hpp>

namespace demo {

class demo_renderer : public sbx::graphics::renderer {

public:

  demo_renderer();

  ~demo_renderer() override = default;

  auto initialize() -> void override;

private:

  sbx::math::color _clear_color;
  sbx::math::color _shadow_map_clear_color;
  sbx::math::vector2u _shadow_map_size;
  sbx::graphics::format _shadow_map_format;

}; // class demo_renderer

} // namespace demo

#endif // DEMO_DEMO_RENDERER_HPP_
