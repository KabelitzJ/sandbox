#ifndef LIBSBX_UI_FONT_HPP_
#define LIBSBX_UI_FONT_HPP_

#include <filesystem>

#include <fmt/format.h>
#include <freetype/freetype.h>

#include <libsbx/utility/primitive.hpp>

#include <libsbx/assets/asset.hpp>

namespace sbx::ui {

struct character {
  
}; // struct character

class pixels : public utility::primitive<std::uint32_t> {

public:

  using super = utility::primitive<std::uint32_t>;

  using super::super;

}; // class pixels

class font : public assets::asset<assets::asset_type::font> {

public:

  font(const std::filesystem::path& path, pixels height = 18u);

  ~font() override;

  auto set_height(pixels height) -> void;

private:

  FT_Face _face;

}; // class font

} // namespace sbx::ui

#endif // LIBSBX_UI_FONT_HPP_
