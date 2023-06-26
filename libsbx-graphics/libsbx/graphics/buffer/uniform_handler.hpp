#ifndef LIBSBX_GRAPHICS_BUFFERS_UNIFORM_HANDLER_HPP_
#define LIBSBX_GRAPHICS_BUFFERS_UNIFORM_HANDLER_HPP_

#include <memory>
#include <optional>

#include <libsbx/core/logger.hpp>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>

#include <libsbx/graphics/buffer/uniform_buffer.hpp>

namespace sbx::graphics {

class uniform_handler {

public:

  explicit uniform_handler(const shader::uniform_block& uniform_block, bool _is_multi_pipeline = false);

  template<typename Type>
	void push(const std::string& uniform_name, const Type& object, std::size_t size = 0);

  auto uniform_buffer() const noexcept -> const uniform_buffer&;

private:

  bool _is_multi_pipeline;
  shader::uniform_block _uniform_block;
  std::vector<std::unique_ptr<graphics::uniform_buffer>> _buffers;

}; // class uniform_handler

} // namespace sbx::graphics

#include <libsbx/graphics/buffer/uniform_handler.ipp>

#endif // LIBSBX_GRAPHICS_BUFFERS_UNIFORM_HANDLER_HPP_
