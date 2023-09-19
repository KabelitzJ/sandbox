#include <libsbx/ui/font.hpp>

#include <filesystem>
#include <ranges>

#include <fmt/format.h>

#include <libsbx/ui/ui_module.hpp>

namespace sbx::ui {

font::font(const std::filesystem::path& path, pixels height) {
  const auto& ui_module = core::engine::get_module<ui::ui_module>();
  const auto& library = ui_module.font_library();

  auto error = FT_Error{0};

  error = FT_New_Face(library, path.string().c_str(), 0, &_face);

  if (error == FT_Err_Unknown_File_Format) {
    throw std::runtime_error{fmt::format("Failed to load font '{}': unknown file format", path.string())};
  } else if (error) {
    throw std::runtime_error{fmt::format("Failed to load font '{}': unknown error", path.string())};
  }

  error = FT_Set_Pixel_Sizes(_face, 0, height);

  if (error) {
    throw std::runtime_error{fmt::format("Failed to set font size to '{}'", height)};
  }

  for (const auto character : std::views::iota(std::uint8_t{0}, std::uint8_t{128})) {
    error = FT_Load_Char(_face, character, FT_LOAD_RENDER);

    if (error) {
      throw std::runtime_error{fmt::format("Failed to load glyph '{}'", character)};
    }

    auto glyph = font::glyph{};

    glyph.size = math::vector2u{_face->glyph->bitmap.width, _face->glyph->bitmap.rows};

    if (glyph.size.x == 0 || glyph.size.y == 0) {
      continue;
    }

    glyph.bearing = math::vector2u{_face->glyph->bitmap_left, _face->glyph->bitmap_top};
    glyph.advance = _face->glyph->advance.x;
    glyph.image = std::make_unique<graphics::image2d>(glyph.size, VK_FORMAT_R8_UNORM);

    glyph.image->set_pixels(_face->glyph->bitmap.buffer);

    _glyphs.insert({character, std::move(glyph)});
  }
}

font::~font() {
  FT_Done_Face(_face);
}

auto font::get_glyph(char character) const noexcept -> const glyph& {
  return _glyphs.at(character);
}

} // namespace sbx::ui
