#ifndef LIBSBX_AUDIO_SOUND_HPP_
#define LIBSBX_AUDIO_SOUND_HPP_

#include <libsbx/assets/asset.hpp>

namespace sbx::audio {

class sound : public assets::asset<assets::asset_type::sound> {

public:

  sound();

  ~sound() override;

private:

}; // class sound

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_SOUND_HPP_
