#ifndef LIBSBX_GRAPHICS_SUBRENDERER_HPP_
#define LIBSBX_GRAPHICS_SUBRENDERER_HPP_

#include <cmath>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>

namespace sbx::graphics {

class subrenderer {

public:

  subrenderer(const render_graph::pass& pass)
  : _pass{pass} { }

  virtual ~subrenderer() = default;

  virtual auto render(command_buffer& command_buffer) -> void = 0;

  auto pass() const noexcept -> const render_graph::pass& {
    return _pass;
  }

private:

  render_graph::pass _pass;

}; // class subrenderer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_SUBRENDERER_HPP_
