#ifndef SBX_EVTSYS_EVENT_LISTENER_HPP_
#define SBX_EVTSYS_EVENT_LISTENER_HPP_

#include "event_queue.hpp"

namespace sbx {

class event_listener {

public:
  event_listener() = default;
  virtual ~event_listener() = default;

private:
  virtual void register_event_callbacks(event_queue& queue) = 0;

};

} // namespace sbx

#endif // SBX_EVTSYS_EVENT_LISTENER_HPP_
