#ifndef LIBSBX_EDITOR_THEMES_HPP_
#define LIBSBX_EDITOR_THEMES_HPP_

#include <unordered_map>
#include <functional>
#include <string>

namespace sbx::editor {

class themes {

public:

  themes();

  void applyTheme(const std::string &theme);
  void addTheme(const std::string &name, const std::function<void()> &callback);
  const std::unordered_map<std::string, std::function<void()>> &getThemes() const;

private:

  static void set_dark_theme_colors();
  static void set_catpuccin_mocha_colors();
  static void set_fluent_ui_colors();
  static void set_bess_dark_colors();
  static void set_modern_dark_colors();

  // theme name and a void callback
  std::unordered_map<std::string, std::function<void()>> m_themes = {};

}; // class themes

} // namespace sbx::editor

#endif // LIBSBX_EDITOR_THEMES_HPP_
