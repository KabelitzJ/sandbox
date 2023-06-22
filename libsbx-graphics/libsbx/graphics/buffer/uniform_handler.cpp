#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>

namespace sbx::graphics {

uniform_handler::uniform_handler(bool _is_multi_pipeline)
: _is_multi_pipeline{_is_multi_pipeline},
  _status{buffer::status::normal} { }

uniform_handler::uniform_handler(const std::optional<shader::uniform_block>& uniform_block, bool _is_multi_pipeline)
: _is_multi_pipeline{_is_multi_pipeline},
  _uniform_block{uniform_block},
  _size{_uniform_block->size()},
  _status{buffer::status::normal} {
    _per_frame_data.resize(swapchain::max_frames_in_flight);

    for (auto& data : _per_frame_data) {
      data.buffer = std::make_unique<graphics::uniform_buffer>(_uniform_block->size());
      data.memory = nullptr;
    }
  }

auto uniform_handler::update(const std::optional<shader::uniform_block>& uniform_block) -> bool {
  const auto current_frame = graphics_module::get().current_frame();

  auto& frame_data = _per_frame_data[current_frame];

  if (_status == buffer::status::reset || (_is_multi_pipeline && !_uniform_block) || (!_is_multi_pipeline && _uniform_block != uniform_block)) {
    if ((_size == 0 && !_uniform_block) || (_uniform_block && _uniform_block != uniform_block && _uniform_block->size() == _size)) {
      _size = uniform_block->size();
    }

    _uniform_block = uniform_block;
    frame_data.buffer = std::make_unique<graphics::uniform_buffer>(_size);
    frame_data.memory = nullptr;
    _status = buffer::status::changed;
    return false;
  }

  if (_status != buffer::status::normal) {
    if (frame_data.memory) {
      frame_data.buffer->unmap();
      frame_data.memory = nullptr;
    }

    _status = buffer::status::normal;
  }

  return true;
}

auto uniform_handler::uniform_buffer() const noexcept -> const graphics::uniform_buffer& {
  auto current_frame = graphics_module::get().current_frame();

  return *_per_frame_data[current_frame].buffer;
}

} // namespace sbx::graphics
