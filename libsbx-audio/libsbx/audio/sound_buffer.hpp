#ifndef LIBSBX_AUDIO_SOUND_BUFFER_HPP_
#define LIBSBX_AUDIO_SOUND_BUFFER_HPP_

#include <filesystem>
#include <concepts>
#include <functional>
#include <type_traits>

#include <AL/al.h>
#include <AL/alc.h>

#include <libsbx/assets/assets.hpp>

#include <libsbx/io/loader_factory.hpp>

namespace sbx::audio {

struct sound_buffer_data {
  std::uint32_t buffer;
}; // struct sound_buffer_data

class sound_buffer : public io::loader_factory<sound_buffer, sound_buffer_data>,  public assets::asset<assets::asset_type::sound> {

public:

  sound_buffer(const std::filesystem::path& path);

  ~sound_buffer() override;

  auto handle() const -> std::uint32_t;

  operator std::uint32_t() const;

private:

  std::uint32_t _buffer;

}; // class sound_buffer

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_SOUND_BUFFER_HPP_
