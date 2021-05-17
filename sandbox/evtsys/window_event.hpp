#ifndef SBX_EVTSYS_WINDOW_EVENT_HPP_
#define SBX_EVTSYS_WINDOW_EVENT_HPP_

#include "event.hpp"

namespace sbx {

class window_event : public event {
  
public:
  window_event() = default;
  virtual ~window_event() = default;

  event_type type() const override;

}; // class window_event

class window_position_event : public window_event {

public:
  window_position_event(int x, int y);
  ~window_position_event() = default;

  int x() const;
  int y() const;

private:
  int _x;
  int _y;

}; // class window_position_event

} // namespace sbx

#endif // SBX_EVTSYS_WINDOW_EVENT_HPP_
