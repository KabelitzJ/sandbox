#include <libsbx/audio/audio_module.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/transform.hpp>

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

  alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

  check_error();

  _gains[sound::type::music] = 1.0f;
  _gains[sound::type::effect] = 1.0f;
  _gains[sound::type::master] = 1.0f;
  _gains[sound::type::ambient] = 1.0f;
  _gains[sound::type::general] = 1.0f;
}

audio_module::~audio_module() {
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(_context);
  alcCloseDevice(_device);
}

auto audio_module::update() -> void {

}

auto audio_module::set_gain(sound::type type, std::float_t gain) -> void {
  _gains[type] = gain;
}

auto audio_module::gain(sound::type type) -> std::float_t {
  return _gains[type];
}

auto audio_module::update_listener_orientation(const math::vector3& position, const math::vector3& forward) -> void {
  alListenerf(AL_GAIN, _gains[sound::type::master]);

  check_error();

  alListener3f(AL_POSITION, position.x, position.y, -position.z);

  check_error();

  alListener3f(AL_VELOCITY, position.x, position.y, -position.z);

  check_error();

  const auto& up = math::vector3::up;

  const auto orientation = std::array<std::float_t, 6>{forward.x, forward.y, forward.z, up.x, up.y, up.z};

  alListenerfv(AL_ORIENTATION, orientation.data());

  check_error();
}

} // namespace sbx::audio
