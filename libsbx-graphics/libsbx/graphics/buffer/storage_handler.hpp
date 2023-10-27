#ifndef LIBSBX_GRAPHICS_BUFFER_STORAGE_HANDLER_HPP_
#define LIBSBX_GRAPHICS_BUFFER_STORAGE_HANDLER_HPP_

#include <memory>
#include <optional>
#include <span>

#include <libsbx/core/logger.hpp>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/graphics/pipeline/shader.hpp>

#include <libsbx/graphics/buffer/storage_buffer.hpp>

namespace sbx::graphics {

class storage_handler {

public:

  storage_handler(const std::optional<shader::uniform_block>& uniform_block = std::nullopt);

  template<typename Type>
  auto push(std::span<const Type> buffer) -> void;

  template<typename Type>
	auto push(const Type& object, std::size_t size, std::size_t offset) -> void;

  template<typename Type>
	auto push(const std::string& uniform_name, const Type& object, std::size_t size = 0) -> void;

  auto storage_buffer() const noexcept -> const graphics::storage_buffer&;

  auto update(const std::optional<shader::uniform_block>& uniform_block) -> bool;

private:

  std::optional<shader::uniform_block> _uniform_block;
  std::unique_ptr<graphics::storage_buffer> _storage_buffer;

}; // class storage_handler

} // namespace sbx::graphics

#include <libsbx/graphics/buffer/storage_handler.ipp>

#endif // LIBSBX_GRAPHICS_BUFFER_STORAGE_HANDLER_HPP_
