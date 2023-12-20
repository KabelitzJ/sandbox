#include <libsbx/audio/loaders/mp3_loader.hpp>

#include <cmath>
#include <fstream>

#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

#include <libsbx/utility/timer.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/audio/audio_module.hpp>

namespace sbx::audio {

auto mp3_loader::load(const std::filesystem::path& path) -> sound_buffer_data {
  auto buffer_data = sound_buffer_data{};

  auto timer = utility::timer{};

  auto file = std::ifstream{path, std::ios::binary};

  if (!file) {
    throw std::runtime_error{"Failed to open file: " + path.string()};
  }

  file.seekg(0, std::ios::end);

  const auto size = file.tellg();

  file.seekg(0, std::ios::beg);

  auto data = std::vector<std::uint8_t>{};
  data.resize(size);

  file.read(reinterpret_cast<char*>(data.data()), size);

  file.close();

  auto config = drmp3_config{};
  auto total_pcm_frame_count = std::uint64_t{};

  auto* sample_data = drmp3_open_memory_and_read_pcm_frames_s16(reinterpret_cast<std::uint8_t*>(data.data()), size, &config, &total_pcm_frame_count, nullptr);

  if (!sample_data) {
    throw std::runtime_error{"Failed to load sound: " + path.string()};
  }

  alGenBuffers(1, &buffer_data.buffer);

  check_error();

  alBufferData(buffer_data.buffer, (config.channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, sample_data, total_pcm_frame_count * config.channels * sizeof(std::int16_t), config.sampleRate);

  check_error();

  core::logger::debug("Loaded sound: {} with {} channel{} ({}), {} sample rate, {} total pcm frame count", path.string(), config.channels, (config.channels > 1) ? "s" : "", (config.channels == 2) ? "stereo" : "mono", config.sampleRate, total_pcm_frame_count);

  return buffer_data;
}

} // namespace sbx::audio
