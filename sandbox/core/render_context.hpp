#ifndef SBX_CORE_RENDER_CONTEXT_HPP_
#define SBX_CORE_RENDER_CONTEXT_HPP_

#include <evtsys/event_queue.hpp>

#include "basic_context.hpp"

namespace sbx {

struct render_context_parameters {
  unsigned int width;
  unsigned int height;
}; // struct render_context_parameters

class render_context : public basic_context {

public:
  render_context();
  ~render_context();

  void poll_events();
  event_queue& queue();

private:
  event_queue* _queue;

}; // class render_context

} // namespace sbx

#endif // SBX_CORE_RENDER_CONTEXT_HPP_