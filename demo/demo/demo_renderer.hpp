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

}; // class demo_renderer

} // namespace demo

#endif // DEMO_DEMO_RENDERER_HPP_
