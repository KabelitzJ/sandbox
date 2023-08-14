#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>

namespace sbx::graphics {

uniform_handler::uniform_handler(const std::optional<shader::uniform_block>& uniform_block)
: _uniform_block{uniform_block} {
  if (_uniform_block) {
    // _buffers.resize(swapchain::max_frames_in_flight);

    // for (auto& buffer : _buffers) {
    //   buffer = std::make_unique<graphics::uniform_buffer>(_uniform_block->size());
    // }
    _uniform_buffer = std::make_unique<graphics::uniform_buffer>(_uniform_block->size());
  }
}

auto uniform_handler::uniform_buffer() const noexcept -> const graphics::uniform_buffer& {
  // auto& graphics_module = core::engine::get_module<graphics::graphics_module>();
  // auto current_frame = graphics_module.current_frame();

  // return *_buffers[current_frame];

  return *_uniform_buffer;
}

auto uniform_handler::update(const std::optional<shader::uniform_block>& uniform_block) -> bool {
  if (_uniform_block != uniform_block) {
    _uniform_block = uniform_block;
    _uniform_buffer = std::make_unique<graphics::uniform_buffer>(_uniform_block->size());

    return false; 
  }

  return true;
}

} // namespace sbx::graphics
