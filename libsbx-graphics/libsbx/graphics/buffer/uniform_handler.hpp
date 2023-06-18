#ifndef LIBSBX_GRAPHICS_BUFFER_UNIFORM_HANDLER_HPP_
#define LIBSBX_GRAPHICS_BUFFER_UNIFORM_HANDLER_HPP_

#include <optional>
#include <memory>

#include <libsbx/graphics/buffer/uniform_buffer.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>

namespace sbx::graphics {

class uniform_handler {

public:



private:

  std::optional<shader::uniform_block> _uniform_block;
  std::uint32_t _size;
  std::unique_ptr<uniform_buffer> _buffer;

}; // class uniform_handler

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFER_UNIFORM_HANDLER_HPP_
