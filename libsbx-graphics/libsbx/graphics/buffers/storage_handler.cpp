#include <libsbx/graphics/buffers/storage_handler.hpp>

#include <fmt/format.h>

namespace sbx::graphics {

storage_handler::storage_handler(VkBufferUsageFlags additional_usage, const std::optional<shader::uniform_block>& uniform_block)
: _uniform_block{uniform_block},
  _additional_usage{additional_usage} {
  if (_uniform_block) {
    _storage_buffer = std::make_unique<graphics::storage_buffer>(graphics::storage_buffer::max_size, _additional_usage);
  }
}

auto storage_handler::storage_buffer() const noexcept -> const graphics::storage_buffer& {
  return *_storage_buffer;
}

auto storage_handler::update(const std::optional<shader::uniform_block>& uniform_block) -> bool {
  if (_uniform_block != uniform_block) {
    _uniform_block = uniform_block;
    _storage_buffer = std::make_unique<graphics::storage_buffer>(graphics::storage_buffer::max_size, _additional_usage);

    return false; 
  }

  return true;
}

} // namespace sbx::graphics
