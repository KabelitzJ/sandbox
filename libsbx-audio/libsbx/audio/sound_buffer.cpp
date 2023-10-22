#include <libsbx/audio/sound_buffer.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

namespace sbx::audio {

sound_buffer::sound_buffer(const std::filesystem::path& path) {
  const auto extension = path.extension().string();

  if (const auto entry = _loaders().find(extension); entry != _loaders().end()) {
    const auto& assets_module = core::engine::get_module<assets::assets_module>();

    const auto actual_path = assets_module.asset_path(path);

    auto& loader = entry->second;

    _buffer = std::invoke(loader, actual_path);
  } else {
   throw std::runtime_error{"Unsupported sound format: " + extension};
  }
}

sound_buffer::~sound_buffer() {

}

auto sound_buffer::handle() const -> handle_type {
  return _buffer;
}

sound_buffer::operator handle_type() const {
  return _buffer;
}

} // namespace sbx::audio
