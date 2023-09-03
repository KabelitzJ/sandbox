#ifndef LIBSBX_UI_FONT_HPP_
#define LIBSBX_UI_FONT_HPP_

#include <filesystem>

#include <fmt/format.h>
#include <freetype/freetype.h>

#include <libsbx/assets/asset.hpp>

namespace sbx::ui {

class font : public assets::asset<assets::asset_type::font> {

public:

  font(const std::filesystem::path& path);

  ~font() override;

private:

  FT_Face _face;

}; // class font

} // namespace sbx::ui

#endif // LIBSBX_UI_FONT_HPP_
