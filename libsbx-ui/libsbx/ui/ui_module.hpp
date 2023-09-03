#ifndef LIBSBX_UI_UI_MODULE_HPP_
#define LIBSBX_UI_UI_MODULE_HPP_

#include <unordered_map>
#include <filesystem>

#include <fmt/format.h>
#include <freetype/freetype.h>

#include <libsbx/core/module.hpp>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/ui/font.hpp>

namespace sbx::ui {

class ui_module : public core::module<ui_module> {

  inline static const auto is_registered = register_module(stage::rendering, dependencies<graphics::graphics_module>{});

public:

  ui_module() {
    const auto error = FT_Init_FreeType(&_library);

    if (error) {
      throw std::runtime_error("Failed to initialize FreeType library");
    }
  }

  ~ui_module() override {
    FT_Done_FreeType(_library);
  }

  auto update() -> void override {

  }

  auto font_library() const noexcept -> const FT_Library& {
    return _library;
  }

private:

  FT_Library _library;

}; // class ui_module

} // namespace sbx::ui

#endif // LIBSBX_UI_UI_MODULE_HPP_
