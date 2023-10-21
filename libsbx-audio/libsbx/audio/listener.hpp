#ifndef LIBSBX_AUDIO_LISTENER_HPP_
#define LIBSBX_AUDIO_LISTENER_HPP_

#include <AL/al.h>
#include <AL/alc.h>

namespace sbx::audio {

class listener {

public:

  listener() {
    
  }

  ~listener() {

  }

private:

  ALuint _listener;

}; // class listener

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_LISTENER_HPP_
