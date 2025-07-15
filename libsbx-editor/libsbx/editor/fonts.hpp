#ifndef LIBSBX_EDITOR_FONTS_HPP_
#define LIBSBX_EDITOR_FONTS_HPP_

#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>

#include <range/v3/all.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/editor/bindings/imgui.hpp>

namespace sbx::editor {

class fonts {

public:

  fonts() = default;

  auto load_font(const std::string& name, const std::filesystem::path& path, std::float_t size) -> void {
    ImGuiIO& io = ImGui::GetIO();

    ImFont* font = io.Fonts->AddFontFromFileTTF(path.string().c_str(), size);

    if (font) {
      _fonts[name] = font;
    } else {
      utility::logger<"editor">::error("Failed to load font: {}", name);
    }
  }

  auto set_active_font(const std::string& name) -> void {
    if (const auto entry = _fonts.find(name); entry != _fonts.end()) {
      _active_font = name;
      ImGui::GetIO().FontDefault = entry->second;
    } else {
      utility::logger<"editor">::error("Font not found: {}", name);
    }
  }

  auto get_fonts() const -> std::vector<std::string> {
    return _fonts | ranges::views::keys | ranges::to<std::vector>();
  }

  auto get_active_font() const -> const std::string& {
    return _active_font;
  }

private:

  std::unordered_map<std::string, ImFont*> _fonts;
  std::string _active_font;

}; // class fonts

} // namespace sbx::editor

#endif // LIBSBX_EDITOR_FONTS_HPP_
