#ifndef LIBSBX_AUDIO_SOUND_BUFFER_HPP_
#define LIBSBX_AUDIO_SOUND_BUFFER_HPP_

#include <filesystem>
#include <concepts>
#include <functional>
#include <type_traits>

#include <AL/al.h>
#include <AL/alc.h>

#include <libsbx/assets/assets.hpp>

namespace sbx::audio {

class sound_buffer : public assets::asset<assets::asset_type::sound> {

public:

  using handle_type = std::uint32_t;

  template<typename Derived>
  class loader {

  protected:

    static auto register_extensions(const std::string& extension) -> bool {
      sound_buffer::_loaders()[extension] = &Derived::load;

      return true;
    }

  }; // struct loader

  sound_buffer(const std::filesystem::path& path);

  ~sound_buffer() override;

  auto handle() const -> handle_type;

  operator handle_type() const;

private:

  using loader_container_type = std::unordered_map<std::string, std::function<handle_type(const std::filesystem::path&)>>;

  static auto _loaders() -> loader_container_type& {
    static auto loaders = loader_container_type{};
    return loaders;
  }

  handle_type _buffer;

}; // class sound_buffer

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_SOUND_BUFFER_HPP_
