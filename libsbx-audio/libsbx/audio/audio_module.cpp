#include <libsbx/audio/audio_module.hpp>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/transform.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

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
  auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

  auto& scene = scenes_module.scene();

  auto camera = scene.camera();

  const auto& transform = camera.get_component<math::transform>();

  const auto& position = transform.position();

  alListener3f(AL_POSITION, position.x, position.y, position.z);

  check_error();

  alListener3f(AL_VELOCITY, position.x, position.y, position.z);

  check_error();

  const auto& forward = transform.forward();
  const auto& up = math::vector3::up;

  const auto orientation = std::array<std::float_t, 6>{forward.x, forward.y, forward.z, up.x, up.y, up.z};

  alListenerfv(AL_ORIENTATION, orientation.data());

  check_error();

  auto sound_components = scene.query<audio::sound>();

  for (auto& sound_component : sound_components) {
    auto& sound = sound_component.get_component<audio::sound>();

    auto& transform = sound_component.get_component<math::transform>();

    const auto& position = transform.position();

    alSource3f(sound.handle(), AL_POSITION, position.x, position.y, position.z);

    check_error();

    alSource3f(sound.handle(), AL_VELOCITY, position.x, position.y, position.z);

    check_error();

    const auto& forward = transform.forward();
    const auto& up = math::vector3::up;

    const auto orientation = std::array<std::float_t, 6>{forward.x, forward.y, forward.z, up.x, up.y, up.z};

    alSourcefv(sound.handle(), AL_ORIENTATION, orientation.data());

    check_error();
  }
}

} // namespace sbx::audio
