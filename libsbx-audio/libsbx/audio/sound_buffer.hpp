#ifndef LIBSBX_AUDIO_SOUND_BUFFER_HPP_
#define LIBSBX_AUDIO_SOUND_BUFFER_HPP_

#include <filesystem>

#include <AL/al.h>
#include <AL/alc.h>

#include <libsbx/assets/assets.hpp>

namespace sbx::audio {

class sound_buffer : public assets::asset<assets::asset_type::sound> {

public:

  sound_buffer(const std::filesystem::path& path);

  ~sound_buffer() override;

  auto handle() const -> ALuint;

  operator ALuint() const;

private:

  ALuint _buffer;

}; // class sound_buffer

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_SOUND_BUFFER_HPP_
