#ifndef SBX_UI_WINDOW_HPP_
#define SBX_UI_WINDOW_HPP_

#include <GLFW/glfw3.h>

#include "context.hpp"

namespace sbx {

class window {

public:

  window();

  window(const window&) = delete;

  window(window&&) = delete;

  ~window();

  window& operator=(const window&) = delete;

  window& operator=(window&&) = delete;

  void swap_buffers();

private:

  void _initialize();
  void _terminate();

  context _context{};
  GLFWwindow* _handle{};

}; // class window

} // namespace sbx

#endif // SBX_UI_WINDOW_HPP_
