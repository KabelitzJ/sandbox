#ifndef SBC_CORE_CONTEXT_HPP_
#define SBC_CORE_CONTEXT_HPP_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace sbx {

template<typename Handle>
class basic_context {

public:
  basic_context();
  basic_context(const basic_context&) = delete;
  basic_context(basic_context&&) = delete;
  virtual ~basic_context();

  basic_context& operator=(const basic_context&) = delete;
  basic_context& operator=(basic_context&&) = delete;

  void make_current();
  Handle handle();

protected:
  Handle _handle;

private:
  static unsigned int _context_counter;

}; // class basic_context

using glfw_context = basic_context<GLFWwindow*>;

} // namespace sbx

#endif // SBC_CORE_CONTEXT_HPP_