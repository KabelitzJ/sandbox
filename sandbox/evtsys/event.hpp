#ifndef SBX_EVTSYS_EVENT_HPP_
#define SBX_EVTSYS_EVENT_HPP_

namespace sbx {

enum class event_type {
  window    = 0,
  keyboard  = 1,
  mouse     = 2
}; // enum class event_type

class event {

public:
  event() = default;
  virtual ~event() = default;

  virtual event_type type() const = 0;

}; // class event

} // namespace sbx

#endif // SBX_EVTSYS_EVENT_HPP_