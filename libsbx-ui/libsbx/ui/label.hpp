#ifndef LIBSBX_UI_LABEL_HPP_
#define LIBSBX_UI_LABEL_HPP_

#include <string>

#include <libsbx/core/logger.hpp>

#include <libsbx/assets/asset.hpp>

#include <libsbx/ui/widget.hpp>

namespace sbx::ui {

class label : public widget {

public:

  label(const std::string& text, const math::vector2& position, const math::vector2& size, assets::asset_id font_id)
  : widget{position, size},
    _text{text},
    _font_id{font_id} { }

  ~label() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {

  }

private:

  std::string _text;
  assets::asset_id _font_id;

}; // class label

} // namespace sbx::ui

#endif // LIBSBX_UI_LABEL_HPP_
