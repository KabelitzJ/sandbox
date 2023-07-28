#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>

namespace sbx::graphics {

uniform_handler::uniform_handler(const shader::uniform_block& uniform_block)
: _uniform_block{uniform_block} {
  _buffers.resize(swapchain::max_frames_in_flight);

  for (auto& buffer : _buffers) {
    buffer = std::make_unique<graphics::uniform_buffer>(_uniform_block.size());
  }
}

auto uniform_handler::uniform_buffer() const noexcept -> const graphics::uniform_buffer& {
  auto current_frame = graphics_module::get().current_frame();

  return *_buffers[current_frame];
}

} // namespace sbx::graphics
