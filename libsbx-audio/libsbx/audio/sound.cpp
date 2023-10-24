#include <libsbx/audio/sound.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/audio/audio_module.hpp>
#include <libsbx/audio/sound_buffer.hpp>

namespace sbx::audio {

sound::sound(assets::asset_id sound_buffer_id, type type, bool should_begin, bool should_loop, std::float_t gain, std::float_t pitch)
: _sound_buffer_id{sound_buffer_id},
  _type{type},
  _gain{gain},
  _pitch{pitch} {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  alGenSources(1, &_source);

  check_error();

  auto& sound_buffer = assets_module.get_asset<audio::sound_buffer>(_sound_buffer_id);

  alSourcei(_source, AL_BUFFER, sound_buffer.handle());

  check_error();

  set_gain(_gain);
  set_pitch(_pitch);

  if (should_begin) {
    play(should_loop);
  }
}

sound::~sound() {
  alDeleteSources(1, &_source);

  check_error();
}

auto sound::handle() const -> std::uint32_t {
  return _source;
}

sound::operator std::uint32_t() const {
  return handle();
}

auto sound::play(bool should_loop) -> void {
  alSourcei(_source, AL_LOOPING, should_loop);

  check_error();

  alSourcePlay(_source);

  check_error();
}

auto sound::set_gain(std::float_t gain) -> void {
  auto& audio_module = core::engine::get_module<audio::audio_module>();
    
  _gain = gain;

  alSourcef(_source, AL_GAIN, _gain * audio_module.gain(_type));

  check_error();
}

auto sound::set_pitch(std::float_t pitch) -> void {
  _pitch = pitch;

  alSourcef(_source, AL_PITCH, _pitch);

  check_error();
}

} // namespace sbx::audio
