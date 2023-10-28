#ifndef LIBSBX_GRAPHICS_BUFFERS_PUSH_HANDLER_HPP_
#define LIBSBX_GRAPHICS_BUFFERS_PUSH_HANDLER_HPP_

#include <cinttypes>
#include <optional>
#include <memory>

#include <libsbx/graphics/pipeline/shader.hpp>
#include <libsbx/graphics/pipeline/pipeline.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::graphics {

class push_handler {

public:

  push_handler(const std::optional<shader::uniform_block>& uniform_block = std::nullopt);

  template<typename Type>
	auto push(const Type& object, std::size_t size, std::size_t offset) -> void;

  template<typename Type>
	auto push(const std::string& uniform_name, const Type& object, std::size_t size = 0) -> void;

  auto update(const std::optional<shader::uniform_block>& uniform_block) -> bool;

  auto bind(command_buffer& command_buffer, const pipeline& pipeline) -> void;

private:

  std::optional<shader::uniform_block> _uniform_block;
  std::unique_ptr<std::uint8_t[]> _data;

}; // class push_handler

} // namespace sbx::graphics

#include <libsbx/graphics/buffers/push_handler.ipp>

#endif // LIBSBX_GRAPHICS_BUFFERS_PUSH_HANDLER_HPP_
