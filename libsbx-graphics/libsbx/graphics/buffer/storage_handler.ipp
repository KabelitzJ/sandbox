#include <libsbx/graphics/buffer/storage_handler.hpp>

namespace sbx::graphics {

template<typename Type>
auto storage_handler::push(std::span<const Type> buffer) -> void {
  if (!_uniform_block || !_storage_buffer) {
    return;
  }

  if (buffer.size() != _uniform_block->size()) {
    throw std::runtime_error{fmt::format("Data size ({}) does not match uniform block size ({})", buffer.size(), _uniform_block->size())};
  }

  _storage_buffer->update(buffer.data(), buffer.size());
}

template<typename Type>
auto storage_handler::push(const Type& object, std::size_t size, std::size_t offset) -> void {
  if (!_uniform_block || !_storage_buffer) {
    return;
  }

  _storage_buffer->update(std::addressof(object), size, offset);
}

template<typename Type>
auto storage_handler::push(const std::string& uniform_name, const Type& object, std::size_t size) -> void {
  if (!_uniform_block || !_storage_buffer) {
    return;
  }

  auto uniform = _uniform_block->find_uniform(uniform_name);

  if (!uniform) {
    throw std::runtime_error{fmt::format("Uniform '{}' not found in uniform block", uniform_name)};
  }

  auto object_size = (size > 0) ? size : std::min(sizeof(Type), static_cast<std::size_t>(uniform->size()));

  push(object, object_size, static_cast<std::size_t>(uniform->offset()));
}

} // namespace sbx::graphics
