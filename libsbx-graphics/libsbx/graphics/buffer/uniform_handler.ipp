#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

template<typename Type>
void uniform_handler::push(const std::string& uniform_name, const Type& object, std::size_t size) {
  const auto current_frame = graphics_module::get().current_frame();

  auto& frame_data = _per_frame_data[current_frame];

  if (!_uniform_block || !frame_data.buffer) {
    return;
  }

  auto uniform = _uniform_block->find_uniform(uniform_name);

  if (!uniform) {
    throw std::runtime_error{fmt::format("Uniform '{}' not found in uniform block", uniform_name)};
  }

  auto real_size = (size > 0) ? size : std::min(sizeof(Type), static_cast<std::size_t>(uniform->size()));

  _push(object, real_size, uniform->offset());
}

template<typename Type>
void uniform_handler::_push(const Type& object, std::size_t size, std::size_t offset) {
  const auto current_frame = graphics_module::get().current_frame();

  auto& frame_data = _per_frame_data[current_frame];

  if (!frame_data.memory) {
    frame_data.memory = frame_data.buffer->map();
  }

  if (_status == buffer::status::changed || std::memcmp(static_cast<std::uint8_t*>(frame_data.memory.get()), std::addressof(object), size) != 0) {
    std::memcpy(static_cast<std::uint8_t*>(frame_data.memory.get()) + offset, &object, size);
    _status = buffer::status::changed;
  }
}

} // namespace sbx::graphics
