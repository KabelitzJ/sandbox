#ifndef LIBSBX_AUDIO_LOADERS_WAVE_LOADER_HPP_
#define LIBSBX_AUDIO_LOADERS_WAVE_LOADER_HPP_

#include <libsbx/audio/sound_buffer.hpp>

namespace sbx::audio {

class wave_loader : public sound_buffer::loader<wave_loader> {

  inline static const auto is_registered = register_extensions(".wav", ".wave");

public:

  static auto load(const std::filesystem::path& path) -> sound_buffer_data;

}; // class wave_loader

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_LOADERS_WAVE_LOADER_HPP_
