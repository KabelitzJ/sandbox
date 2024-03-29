#include <libsbx/graphics/buffers/push_handler.hpp>

namespace sbx::graphics {

push_handler::push_handler(const std::optional<shader::uniform_block>& uniform_block)
: _uniform_block{uniform_block} {
  if (_uniform_block) {
    _data = std::make_unique<std::uint8_t[]>(_uniform_block->size());
  }
}

auto push_handler::update(const std::optional<shader::uniform_block>& uniform_block) -> bool {
  if (_uniform_block != uniform_block) {
    _uniform_block = uniform_block;
    _data = std::make_unique<uint8_t[]>(_uniform_block->size());

    return false; 
  }

  return true;
}

auto push_handler::bind(command_buffer& command_buffer, const pipeline& pipeline) -> void {
  vkCmdPushConstants(command_buffer, pipeline.layout(), _uniform_block->stage_flags(), 0, _uniform_block->size(), _data.get());
}

} // namespace sbx::graphics
