#include "context.hpp"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include <core/logger.hpp>

namespace sbx {

context::context() {
  ++_use_count;

  if (_use_count == 1) {
    _initialize();
  }
}

context::~context() {
  --_use_count;

  if (_use_count == 0) {
    _terminate();
  }
}

void context::_initialize() {
  if (!glfwInit()) {
    logger::critical("Failed to initialize GLFW");
    throw std::runtime_error("Failed to initialize GLFW");
  }
}

void context::_terminate() {
  glfwTerminate();
}

} // namespace sbx
