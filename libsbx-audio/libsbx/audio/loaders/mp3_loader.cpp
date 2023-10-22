#include <libsbx/audio/loaders/mp3_loader.hpp>

#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

namespace sbx::audio {

auto mp3_loader::load(const std::filesystem::path& path) -> sound_buffer_data{
  auto buffer_data = sound_buffer_data{};

  return buffer_data;
}

} // namespace sbx::audio
