#include <libsbx/audio/audio_module.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

namespace sbx::audio {

auto check_error() -> void {
  auto error = alGetError();

  switch (error) {
    case AL_INVALID_NAME:
      throw std::runtime_error{"OpenAL error: Invalid name"};
    case AL_INVALID_ENUM:
      throw std::runtime_error{"OpenAL error: Invalid enum"};
    case AL_INVALID_VALUE:
      throw std::runtime_error{"OpenAL error: Invalid value"};
    case AL_INVALID_OPERATION:
      throw std::runtime_error{"OpenAL error: Invalid operation"};
    case AL_OUT_OF_MEMORY:
      throw std::runtime_error{"OpenAL error: Out of memory"};
    case AL_NO_ERROR:
    default:
      break;
  }
}

audio_module::audio_module() {
  _device = alcOpenDevice(nullptr);

  if (!_device) {
    throw std::runtime_error{"Failed to open OpenAL device"};
  }

  _context = alcCreateContext(_device, nullptr);

  if (!alcMakeContextCurrent(_context)) {
    throw std::runtime_error{"Failed to create OpenAL context"};
  }

  check_error();
}

audio_module::~audio_module() {
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(_context);
  alcCloseDevice(_device);
}

auto audio_module::update() -> void {

}

auto audio_module::load_sound() -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  auto buffer = sound_buffer{assets_module.asset_path("res://audio/ambient.wav")};
}

} // namespace sbx::audio
