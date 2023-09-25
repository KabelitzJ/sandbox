#ifndef LIBSBX_UI_FONT_HPP_
#define LIBSBX_UI_FONT_HPP_

#include <filesystem>
#include <unordered_map>

#include <libsbx/utility/primitive.hpp>

#include <libsbx/assets/asset.hpp>

#include <libsbx/math/vector2.hpp>

#include <libsbx/graphics/images/image2d.hpp>

#include <libsbx/ui/atlas.hpp>

namespace sbx::ui {

template<typename Type>
struct basic_rect {
  math::basic_vector2<Type> position;
  math::basic_vector2<Type> size;
}; // struct basic_rect

using rect = basic_rect<std::float_t>;
using rectu = basic_rect<std::uint32_t>;

class font : public assets::asset<assets::asset_type::font> {

public:

  struct glyph_info {
    rectu bounds;
    rect uvs;
    math::vector2 bearing;
    std::uint32_t advance;
  }; // struct glyph_info

  font(const std::filesystem::path& path, std::uint32_t height = 32u);

  ~font() override;

  auto glyph(char character) const noexcept -> const glyph_info&;

  auto atlas() const noexcept -> const ui::atlas&;

private:

  std::unordered_map<char, glyph_info> _glyphs;
  std::unique_ptr<ui::atlas> _atlas;

}; // class font

} // namespace sbx::ui

#endif // LIBSBX_UI_FONT_HPP_
