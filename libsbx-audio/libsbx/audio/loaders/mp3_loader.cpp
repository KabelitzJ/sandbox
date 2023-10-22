#include <libsbx/audio/loaders/mp3_loader.hpp>

#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

namespace sbx::audio {

auto mp3_loader::load(const std::filesystem::path& path) -> sound_buffer::handle_type {
  return 0;
}

} // namespace sbx::audio
