#ifndef LIBSBX_UI_FONT_HPP_
#define LIBSBX_UI_FONT_HPP_

#include <filesystem>
#include <unordered_map>

#include <freetype/freetype.h>

#include <libsbx/utility/primitive.hpp>

#include <libsbx/assets/asset.hpp>

#include <libsbx/math/vector2.hpp>

#include <libsbx/graphics/images/image2d.hpp>

namespace sbx::ui {

struct character {
  
}; // struct character

struct pixels : utility::primitive<std::uint32_t> { 
  using super = utility::primitive<std::uint32_t>;
  using super::super;
};// struct pixels

class font : public assets::asset<assets::asset_type::font> {

public:

  struct glyph {
    math::vector2u size;
    math::vector2u bearing;
    std::uint32_t advance;
    std::unique_ptr<graphics::image2d> image;
  }; // struct glyph

  font(const std::filesystem::path& path, pixels height = 18u);

  ~font() override;

  auto get_glyph(char character) const noexcept -> const glyph&;

private:

  FT_Face _face;

  std::unordered_map<char, glyph> _glyphs;

}; // class font

} // namespace sbx::ui

#endif // LIBSBX_UI_FONT_HPP_
