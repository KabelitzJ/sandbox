#ifndef LIBSBX_AUDIO_SOUND_HPP_
#define LIBSBX_AUDIO_SOUND_HPP_

#include <AL/al.h>
#include <AL/alc.h>

#include <libsbx/assets/asset.hpp>

namespace sbx::audio {

class sound : public assets::asset<assets::asset_type::sound> {

public:

  sound(assets::asset_id sound_buffer, bool should_begin = false, bool is_looping = false, std::float_t gain = 1.0f, std::float_t pitch = 1.0f);

  ~sound() override;

  auto play(bool is_looping = false) -> void;

  auto set_gain(std::float_t gain) -> void;

  auto set_pitch(std::float_t pitch) -> void;

private:

  assets::asset_id _sound_buffer_id;

  std::uint32_t _source;

  std::float_t _gain;
  std::float_t _pitch;

}; // class sound

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_SOUND_HPP_
