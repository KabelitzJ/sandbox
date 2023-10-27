#include <libsbx/graphics/buffer/storage_handler.hpp>

namespace sbx::graphics {

storage_handler::storage_handler(const std::optional<shader::uniform_block>& uniform_block)
: _uniform_block{uniform_block} {
  if (_uniform_block) {
    _storage_buffer = std::make_unique<graphics::storage_buffer>(_uniform_block->size());
  }
}

auto push(std::span<const std::byte> data) -> void {
  if (!_uniform_block || !_storage_buffer) {
    return;
  }

  if (data.size() != _uniform_block->size()) {
    throw std::runtime_error{fmt::format("Data size ({}) does not match uniform block size ({})", data.size(), _uniform_block->size())};
  }

  _storage_buffer->update(data);
}

auto storage_handler::storage_buffer() const noexcept -> const graphics::storage_buffer& {
  return *_storage_buffer;
}

auto storage_handler::update(const std::optional<shader::uniform_block>& uniform_block) -> bool {
  if (_uniform_block != uniform_block) {
    _uniform_block = uniform_block;
    _storage_buffer = std::make_unique<graphics::storage_buffer>(_uniform_block->size());

    return false; 
  }

  return true;
}

} // namespace sbx::graphics
