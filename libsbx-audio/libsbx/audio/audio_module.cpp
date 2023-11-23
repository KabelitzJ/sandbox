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

  alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

  check_error();

  _gains[sound::type::music] = 1.0f;
  _gains[sound::type::effect] = 1.0f;
  _gains[sound::type::master] = 1.0f;
  _gains[sound::type::ambient] = 1.0f;
  _gains[sound::type::general] = 1.0f;

  auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

  scenes_module.register_component_loader("sound_source", [](scenes::node& node, const YAML::Node& node_data) {
    const auto sound = node_data["sound"].as<std::string>();
    const auto is_looping = node_data["is_looping"].as<bool>();
    const auto volume = node_data["volume"].as<std::float_t>();

    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto sound_id = assets_module.try_get_asset_id(std::filesystem::path{sound});

    if (!sound_id) {
      core::logger::warn("Sound '{}' could not be found", sound);
      return;
    }

    node.add_component<audio::sound>(*sound_id, audio::sound::type::ambient, true, is_looping, volume);
  });
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

  const auto& camera_transform = camera.get_component<math::transform>();

  const auto& camera_position = camera_transform.position();

  alListenerf(AL_GAIN, _gains[sound::type::master]);

  check_error();

  alListener3f(AL_POSITION, camera_position.x, camera_position.y, -camera_position.z);

  check_error();

  alListener3f(AL_VELOCITY, camera_position.x, camera_position.y, -camera_position.z);

  check_error();

  const auto& camera_forward = camera_transform.forward();
  const auto& camera_up = math::vector3::up;

  const auto camera_orientation = std::array<std::float_t, 6>{camera_forward.x, camera_forward.y, camera_forward.z, camera_up.x, camera_up.y, camera_up.z};

  alListenerfv(AL_ORIENTATION, camera_orientation.data());

  check_error();

  auto sound_components = scene.query<audio::sound>();

  for (auto& sound_component : sound_components) {
    auto& sound = sound_component.get_component<audio::sound>();

    auto& sound_transform = sound_component.get_component<math::transform>();

    const auto& sound_position = sound_transform.position();

    alSource3f(sound, AL_POSITION, sound_position.x, sound_position.y, -sound_position.z);

    check_error();

    alSource3f(sound, AL_VELOCITY, sound_position.x, sound_position.y, -sound_position.z);

    check_error();

    const auto& sound_forward = sound_transform.forward();
    const auto& sound_up = math::vector3::up;

    const auto sound_orientation = std::array<std::float_t, 6>{sound_forward.x, sound_forward.y, sound_forward.z, sound_up.x, sound_up.y, sound_up.z};

    alSourcefv(sound, AL_ORIENTATION, sound_orientation.data());

    check_error();
  }
}

auto audio_module::set_gain(sound::type type, std::float_t gain) -> void {
  _gains[type] = gain;
}

auto audio_module::gain(sound::type type) -> std::float_t {
  return _gains[type];
}

} // namespace sbx::audio
