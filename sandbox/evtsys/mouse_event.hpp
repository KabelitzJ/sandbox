#ifndef SBX_EVTSYS_MOUSE_EVENT_HPP_
#define SBX_EVTSYS_MOUSE_EVENT_HPP_

#include "event.hpp"

namespace sbx {

class mouse_moved_event : public event {

public:
  mouse_moved_event(float x, float y);
  ~mouse_moved_event() = default;

  event_type type() const override;

  float x() const;
  float y() const;

private:
  float _x;
  float _y;

}; // class mouse_moved_event

} // namespace sbx

#endif // SBX_EVTSYS_MOUSE_EVENT_HPP_
