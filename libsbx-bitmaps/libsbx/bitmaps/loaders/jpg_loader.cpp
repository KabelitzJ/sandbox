#include <libsbx/bitmaps/loaders/jpg_loader.hpp>

#include <stb_image.h>

namespace sbx::bitmaps {

auto jpg_loader::load(const std::filesystem::path& path) -> bitmap_data {
  auto data = bitmap_data{};

  // data.buffer = jp

  return data;
}

} // namespace sbx::bitmaps
