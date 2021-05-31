#ifndef SBX_EVTSYS_EVENT_HPP_
#define SBX_EVTSYS_EVENT_HPP_

namespace sbx {

/**
 * @class event
 * 
 * @brief Describes as basic event
 */
struct event {
  event() = default;
  virtual ~event() = default;
}; // class event

} // namespace sbx

#endif // SBX_EVTSYS_EVENT_HPP_