#ifndef LIBSBX_UI_WIDGET_HPP_
#define LIBSBX_UI_WIDGET_HPP_

#include <cinttypes>

#include <libsbx/math/vector2.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::ui {

class widget {

public:

  widget(const math::vector2& position, const math::vector2& size)
  : _position{position},
    _size{size} { }

  virtual ~widget() = default;

  virtual auto render(graphics::command_buffer& command_buffer) -> void = 0;

  auto position() const noexcept -> const math::vector2& {
    return _position;
  }

  auto set_position(const math::vector2& position) noexcept -> void {
    _position = position;
  }

  auto size() const noexcept -> const math::vector2& {
    return _size;
  }

  auto set_size(const math::vector2& size) noexcept -> void {
    _size = size;
  }

private:
  
  math::vector2 _position;
  math::vector2 _size;

}; // class widget

} // namespace sbx::ui

#endif // LIBSBX_UI_WIDGET_HPP_
