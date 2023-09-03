#ifndef LIBSBX_UI_LABEL_HPP_
#define LIBSBX_UI_LABEL_HPP_

#include <string>

#include <libsbx/ui/widget.hpp>

namespace sbx::ui {

class label : public widget {

public:

  label(const std::string& text, const math::vector2& position, const math::vector2& size)
  : widget{position, size},
    _text{text} { }

  ~label() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {

  }

private:

  std::string _text;

}; // class label

} // namespace sbx::ui

#endif // LIBSBX_UI_LABEL_HPP_
