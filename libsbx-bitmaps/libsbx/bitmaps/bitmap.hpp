#ifndef LIBSBX_BITMAPS_BITMAP_HPP_
#define LIBSBX_BITMAPS_BITMAP_HPP_

#include <cinttypes>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

#include <libsbx/io/loader_factory.hpp>

namespace sbx::bitmaps {

struct bitmap_data {
  std::uint32_t width;
  std::uint32_t height;
  std::uint8_t* buffer;
}; // struct bitmap_data

class bitmap : public io::loader_factory<bitmap, bitmap_data> {

public:

  bitmap(const std::filesystem::path& path);

  ~bitmap();

private:

}; // class bitmap

} // namespace sbx::bitmaps

#endif // LIBSBX_BITMAPS_BITMAP_HPP_
