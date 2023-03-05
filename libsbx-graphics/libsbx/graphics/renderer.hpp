#ifndef LIBSBX_GRAPHICS_RENDERER_HPP_
#define LIBSBX_GRAPHICS_RENDERER_HPP_

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::graphics {

class renderer {

public:

  renderer() = default;

  virtual ~renderer() = default;

  virtual auto render(command_buffer& command_buffer) -> void = 0;

private:

}; // class renderer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDERER_HPP_
