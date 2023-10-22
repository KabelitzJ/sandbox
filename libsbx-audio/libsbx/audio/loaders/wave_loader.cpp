#include <libsbx/audio/loaders/wave_loader.hpp>

#include <cmath>
#include <fstream>

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

#include <libsbx/utility/timer.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/audio/audio_module.hpp>

namespace sbx::audio {

auto wave_loader::load(const std::filesystem::path& path) -> sound_buffer::handle_type {
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

  auto buffer = sound_buffer::handle_type{};
  auto channels = std::uint32_t{};
  auto sample_rate = std::uint32_t{};
  auto total_pcm_frame_count = std::uint64_t{};

  auto* sample_data = drwav_open_memory_and_read_pcm_frames_s16(reinterpret_cast<std::uint8_t*>(data.data()), size, &channels, &sample_rate, &total_pcm_frame_count, nullptr);

  if (!sample_data) {
    throw std::runtime_error{"Failed to load sound: " + path.string()};
  }

  alGenBuffers(1, &buffer);

  check_error();

  alBufferData(buffer, (channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, sample_data, total_pcm_frame_count * channels * sizeof(std::int16_t), sample_rate);

  check_error();

  core::logger::debug("Loaded sound: {} with {} channel{} ({}), {} sample rate, {} total pcm frame count", path.string(), channels, (channels > 1) ? "s" : "", (channels == 2) ? "stereo" : "mono", sample_rate, total_pcm_frame_count);

  return buffer;
}

} // namespace sbx::audio
