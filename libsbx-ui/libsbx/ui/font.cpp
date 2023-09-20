#include <libsbx/ui/font.hpp>

#include <cmath>

#include <algorithm>
#include <filesystem>
#include <ranges>

#include <fmt/format.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <libsbx/ui/ui_module.hpp>

namespace sbx::ui {

static constexpr auto num_glyphs = 128u;

font::font(const std::filesystem::path& path, std::uint32_t height) {
  const auto& ui_module = core::engine::get_module<ui::ui_module>();
  const auto& library = ui_module.font_library();

  auto error = FT_Error{0};

  error = FT_New_Face(library, path.string().c_str(), 0, &_face);

  if (error == FT_Err_Unknown_File_Format) {
    throw std::runtime_error{fmt::format("Failed to load font '{}': unknown file format", path.string())};
  } else if (error) {
    throw std::runtime_error{fmt::format("Failed to load font '{}': unknown error", path.string())};
  }

  error = FT_Set_Char_Size(_face, 0, height << 6, 96, 96);

  if (error) {
    throw std::runtime_error{fmt::format("Failed to set font size to '{}'", height)};
  }

  auto atlas_size = math::vector2u{0, 0};

  for (const auto character : std::views::iota(0u, num_glyphs)) {
    error = FT_Load_Char(_face, character, FT_LOAD_RENDER);

    if (error) {
      throw std::runtime_error{fmt::format("Failed to load glyph '{}'", character)};
    }

    atlas_size.x += _face->glyph->bitmap.width;
    atlas_size.y = std::max(atlas_size.y, _face->glyph->bitmap.rows);
  }

  core::logger::debug("Creating font atlas for font '{}' ({}x{})", path.string(), atlas_size.x, atlas_size.y);

  auto atlas = std::vector<std::uint8_t>{};
  atlas.resize(atlas_size.x * atlas_size.y);

  auto atlas_position = math::vector2u{0, 0};

  for (const auto character : std::views::iota(0u, num_glyphs)) {
    error = FT_Load_Char(_face, character, FT_LOAD_RENDER);

    if (error) {
      throw std::runtime_error{fmt::format("Failed to load glyph '{}'", character)};
    }

    const auto glyph = _face->glyph;

    for (auto y = 0u; y < glyph->bitmap.rows; ++y) {
      for (auto x = 0u; x < glyph->bitmap.width; ++x) {
        const auto atlas_index = (atlas_position.y + y) * atlas_size.x + (atlas_position.x + x);
        const auto glyph_index = y * glyph->bitmap.width + x;

        atlas[atlas_index] = glyph->bitmap.buffer[glyph_index];
      }
    }

    auto glyph_info = font::glyph_info{};

    glyph_info.bounds.position.x = atlas_position.x;
    glyph_info.bounds.position.y = atlas_position.y;
    glyph_info.bounds.size.x = glyph->bitmap.width;
    glyph_info.bounds.size.y = glyph->bitmap.rows;

    glyph_info.uvs.position.x = static_cast<std::float_t>(atlas_position.x) / atlas_size.x;
    glyph_info.uvs.position.y = static_cast<std::float_t>(atlas_position.y) / atlas_size.y;
    glyph_info.uvs.size.x = static_cast<std::float_t>(glyph->bitmap.width) / atlas_size.x;
    glyph_info.uvs.size.y = static_cast<std::float_t>(glyph->bitmap.rows) / atlas_size.y;

    glyph_info.bearing.x = glyph->bitmap_left;
    glyph_info.bearing.y = glyph->bitmap_top;

    glyph_info.advance = glyph->advance.x >> 6;

    _glyphs.insert({character, glyph_info});

    atlas_position.x += glyph->bitmap.width;
  }

  auto png_data = std::vector<std::uint8_t>{};
  png_data.resize(atlas_size.x * atlas_size.y * 4);

  for (const auto i : std::views::iota(0u, atlas_size.x * atlas_size.y)) {
    png_data[i * 4 + 0] = atlas[i];
    png_data[i * 4 + 1] = atlas[i];
    png_data[i * 4 + 2] = atlas[i];
    png_data[i * 4 + 3] = 0xff;
  }

  stbi_write_png("atlas.png", atlas_size.x, atlas_size.y, 4, png_data.data(), atlas_size.x * 4);

  _atlas = std::make_unique<graphics::image2d>(atlas_size, VK_FORMAT_R8_UNORM);
  _atlas->set_pixels(atlas.data());
}

font::~font() {
  FT_Done_Face(_face);
}

auto font::glyph(char character) const noexcept -> const glyph_info& {
  return _glyphs.at(character);
}

auto font::atlas() const noexcept -> const graphics::image2d& {
  return *_atlas;
}

} // namespace sbx::ui
