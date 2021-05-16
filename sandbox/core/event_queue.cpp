#include "event_queue.hpp"

namespace sbx {

event_queue::event_queue(GLFWwindow* context) : _context(context), _queue() {
  glfwSetWindowUserPointer(_context, this);
}

event_queue::~event_queue() {
  glfwSetWindowUserPointer(_context, nullptr);
}

} // namespace sbx
