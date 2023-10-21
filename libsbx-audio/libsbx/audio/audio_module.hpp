#ifndef LIBSBX_AUDIO_AUDIO_MODULE_HPP_
#define LIBSBX_AUDIO_AUDIO_MODULE_HPP_

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

private:

  ALCdevice* _device;
  ALCcontext* _context;

}; // class audio_module

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_AUDIO_MODULE_HPP_
