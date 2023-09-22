#ifndef LIBSBX_UI_WIDGET_HPP_
#define LIBSBX_UI_WIDGET_HPP_

#include <cinttypes>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/uuid.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

namespace sbx::ui {

class widget {

public:

  widget(const math::vector2u& position, const math::vector2u& size)
  : _position{position},
    _size{size} { }

  virtual ~widget() = default;

  virtual auto update(graphics::uniform_handler& uniform, graphics::descriptor_handler& descriptor_handler) -> void = 0;

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

  auto id() const noexcept -> const math::uuid& {
    return _id;
  }

private:
  
  math::vector2 _position;
  math::vector2 _size;
  math::uuid _id;

}; // class widget

} // namespace sbx::ui

#endif // LIBSBX_UI_WIDGET_HPP_
