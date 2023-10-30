#ifndef LIBSBX_AUDIO_SOUND_HPP_
#define LIBSBX_AUDIO_SOUND_HPP_

#include <AL/al.h>
#include <AL/alc.h>

#include <libsbx/math/vector3.hpp>

#include <libsbx/assets/asset.hpp>

namespace sbx::audio {

class sound : public assets::asset<assets::asset_type::sound> {

public:

  enum class type : std::uint8_t {
    master,
    general,
    music,
    effect,
    ambient
  }; // enum class type

  sound(assets::asset_id sound_buffer, type type = type::general, bool should_begin = false, bool should_loop = false, std::float_t gain = 1.0f, std::float_t pitch = 1.0f);

  ~sound() override;

  auto handle() const -> std::uint32_t;

  operator std::uint32_t() const;

  auto play(bool is_looping = false) -> void;

  auto set_gain(std::float_t gain) -> void;

  auto set_pitch(std::float_t pitch) -> void;

  auto update_orientations(const math::vector3& position, const math::vector3& forward) -> void;

private:

  assets::asset_id _sound_buffer_id;

  std::uint32_t _source;

  type _type;

  std::float_t _gain;
  std::float_t _pitch;

}; // class sound

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_SOUND_HPP_
