#ifndef LIBSBX_AUDIO_AUDIO_MODULE_HPP_
#define LIBSBX_AUDIO_AUDIO_MODULE_HPP_

#include <unordered_map>

#include <AL/al.h>
#include <AL/alc.h>

#include <libsbx/core/module.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <libsbx/audio/sound.hpp>
#include <libsbx/audio/sound_buffer.hpp>

namespace sbx::audio {

auto check_error() -> void;

class audio_module : public core::module<audio_module> {

  inline static const auto is_registered = register_module(stage::pre, dependencies<assets::assets_module>{});

public:

  audio_module();

  ~audio_module() override;

  auto update() -> void override;

  auto set_gain(sound::type type, std::float_t gain) -> void;

  auto gain(sound::type type) -> std::float_t;

private:

  ALCdevice* _device;
  ALCcontext* _context;

  std::unordered_map<sound::type, std::float_t> _gains;

}; // class audio_module

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_AUDIO_MODULE_HPP_
