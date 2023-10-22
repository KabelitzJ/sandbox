#ifndef LIBSBX_AUDIO_LOADERS_MP3_LOADER_HPP_
#define LIBSBX_AUDIO_LOADERS_MP3_LOADER_HPP_

#include <libsbx/audio/sound_buffer.hpp>

namespace sbx::audio {

class mp3_loader : public sound_buffer::loader<mp3_loader> {

  inline static const auto is_registered = register_extensions(".mp3");

public:

  static auto load(const std::filesystem::path& path) -> sound_buffer_data;

}; // class mp3_loader

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_LOADERS_MP3_LOADER_HPP_
