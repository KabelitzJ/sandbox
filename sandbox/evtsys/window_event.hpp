#ifndef SBX_EVTSYS_WINDOW_EVENT_HPP_
#define SBX_EVTSYS_WINDOW_EVENT_HPP_

#include "event.hpp"

namespace sbx {

class window_moved_event : public event {

public:
  window_moved_event(int x, int y);
  ~window_moved_event() = default;

  event_type type() const override;

  int x() const;
  int y() const;

private:
  int _x;
  int _y;

}; // class window_moved_event


class window_resized_event : public event {

public:
  window_resized_event(int width, int height);
  ~window_resized_event() = default;

  event_type type() const override;

  int width() const;
  int height() const;

private:
  int _width;
  int _height;

}; // class window_resized_event

class window_closed_event : public event {

public:
  window_closed_event() = default;
  ~window_closed_event() = default;

  event_type type() const override;

}; // class window_closed_event

class window_refreshed_event : public event {

public:
  window_refreshed_event() = default;
  ~window_refreshed_event() = default;

  event_type type() const override;

}; // class window_refreshed_event

class framebuffer_resized_event : public event {

public:
  framebuffer_resized_event(int width, int height);
  ~framebuffer_resized_event() = default;

  event_type type() const override;

  int width() const;
  int height() const;

private:
  int _width;
  int _height;

}; // class framebuffer_resized_event

} // namespace sbx

#endif // SBX_EVTSYS_WINDOW_EVENT_HPP_
