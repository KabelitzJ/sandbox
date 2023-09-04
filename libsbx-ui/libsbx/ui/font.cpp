#include <libsbx/ui/font.hpp>

#include <filesystem>

#include <libsbx/ui/ui_module.hpp>

namespace sbx::ui {

font::font(const std::filesystem::path& path, pixels height) {
  const auto& ui_module = core::engine::get_module<ui::ui_module>();
  const auto& library = ui_module.font_library();

  const auto error = FT_New_Face(library, path.string().c_str(), 0, &_face);

  if (error == FT_Err_Unknown_File_Format) {
    throw std::runtime_error{fmt::format("Failed to load font '{}': unknown file format", path.string())};
  } else if (error) {
    throw std::runtime_error{fmt::format("Failed to load font '{}': unknown error", path.string())};
  }

  FT_Set_Pixel_Sizes(_face, 0, height);
}

font::~font() {
  FT_Done_Face(_face);
}

auto font::set_height(pixels height) -> void {
  FT_Set_Pixel_Sizes(_face, 0, height);
}

} // namespace sbx::ui
