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

  uniform_handler(const std::optional<shader::uniform_block>& uniform_block = std::nullopt);

  template<typename Type>
	auto push(const Type& object, std::size_t size, std::size_t offset) -> void;

  template<typename Type>
	auto push(const std::string& uniform_name, const Type& object, std::size_t size = 0) -> void;

  auto uniform_buffer() const noexcept -> const uniform_buffer&;

  auto update(const std::optional<shader::uniform_block>& uniform_block) -> bool;

private:

  std::optional<shader::uniform_block> _uniform_block;
  std::unique_ptr<graphics::uniform_buffer> _uniform_buffer;

}; // class uniform_handler

} // namespace sbx::graphics

#include <libsbx/graphics/buffer/uniform_handler.ipp>

#endif // LIBSBX_GRAPHICS_BUFFERS_UNIFORM_HANDLER_HPP_
