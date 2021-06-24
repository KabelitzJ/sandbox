#include "basic_context.hpp"

#include <glad/glad.h>

namespace sbx {

basic_context::basic_context() : _handle(nullptr) {
  if (_context_counter == 0) {
    glfwInit();
  }

  ++_context_counter;
}

basic_context::~basic_context() {
  --_context_counter;

  if (_context_counter == 0) {
    glfwTerminate();
  }
}

void basic_context::make_current() {
  glfwMakeContextCurrent(_handle);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

GLFWwindow* basic_context::handle() {
  return _handle;
}

} // namespace sbx
