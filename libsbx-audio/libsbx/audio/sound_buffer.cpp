#include <libsbx/audio/sound_buffer.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

namespace sbx::audio {

sound_buffer::sound_buffer(const std::filesystem::path& path) {
  const auto extension = path.extension().string();

  const auto entry = _loaders().find(extension);

  if (entry == _loaders().end()) {
    throw std::runtime_error{"No loader found for extension: " + extension};
  }

  auto& loader = entry->second;

  const auto data = std::invoke(loader, path);

  _buffer = data.buffer;
}

sound_buffer::~sound_buffer() {

}

auto sound_buffer::handle() const -> std::uint32_t {
  return _buffer;
}

sound_buffer::operator std::uint32_t() const {
  return _buffer;
}

} // namespace sbx::audio
