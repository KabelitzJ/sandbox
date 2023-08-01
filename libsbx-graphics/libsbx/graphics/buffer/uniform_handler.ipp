#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

template<typename Type>
void uniform_handler::push(const std::string& uniform_name, const Type& object, std::size_t size) {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto current_frame = graphics_module.current_frame();

  auto& buffer = _buffers[current_frame];

  auto uniform = _uniform_block.find_uniform(uniform_name);

  if (!uniform) {
    throw std::runtime_error{fmt::format("Uniform '{}' not found in uniform block", uniform_name)};
  }

  auto real_size = (size > 0) ? size : std::min(sizeof(Type), static_cast<std::size_t>(uniform->size()));

  buffer->write(&object, real_size, uniform->offset());
}

} // namespace sbx::graphics
