#include "window_event.hpp"

namespace sbx {

// window_moved_event

window_moved_event::window_moved_event(int x, int y)
: _x(x),
  _y(y) {

}

event_type window_moved_event::type() const {
  return event_type::WINDOW_MOVED;
}

int window_moved_event::x() const {
  return _x;
}

int window_moved_event::y() const {
  return _y;
}

// window_resized_event

window_resized_event::window_resized_event(int width, int height)
: _width(width),
  _height(height) {

}

event_type window_resized_event::type() const {
  return event_type::WINDOW_RESIZED;
}

int window_resized_event::width() const {
  return _width;
}

int window_resized_event::height() const {
  return _height;
}

// window_closed_event

event_type window_closed_event::type() const {
  return event_type::WINDOW_CLOSED;
}

// window_refresh_event

event_type window_refreshed_event::type() const {
  return event_type::WINDOW_REFRESHED;
}





// framebuffer_resized_event

framebuffer_resized_event::framebuffer_resized_event(int width, int height)
: _width(width),
  _height(height) {

}

event_type framebuffer_resized_event::type() const {
  return event_type::WINDOW_RESIZED;
}

int framebuffer_resized_event::width() const {
  return _width;
}

int framebuffer_resized_event::height() const {
  return _height;
}

} // namespace sbx
