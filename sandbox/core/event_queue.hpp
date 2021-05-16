#ifndef SBX_CORE_EVENT_QUEUE_HPP_
#define SBX_CORE_EVENT_QUEUE_HPP_

#include <queue>

#include <GLFW/glfw3.h>

namespace sbx {

class event_queue {

public:
  event_queue(GLFWwindow* context);
  ~event_queue();

private:
  GLFWwindow* _context;
  std::queue<int> _queue;

}; // class event_queue

} // namespace sbx

#endif // SBX_CORE_EVENT_QUEUE_HPP_
