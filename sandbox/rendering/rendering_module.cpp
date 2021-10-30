#include "rendering_module.hpp"

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "render_system.hpp"

namespace sbx {

rendering_module::rendering_module() {
  
}

void rendering_module::initialize() {
  _logger->info("Initializing rendering module...");

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    _logger->critical("Could not initialize glad");
    return;
  }

  _log_context_info();

  glClearColor(0.95f, 0.5f, 0.43f, 1.0f);
  
  _scheduler->add_system<render_system>(_event_queue);
}

void rendering_module::terminate() {
  _logger->info("Terminating rendering module...");
}

void rendering_module::_log_context_info() const {
  const auto* vendor = glGetString(GL_VENDOR);
  const auto* renderer = glGetString(GL_RENDERER);
  const auto* version = glGetString(GL_VERSION);
  const auto* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

  _logger->info("======================== context info ========================");
  _logger->info("gpu vendor:\t{}", vendor);
  _logger->info("gpu:\t\t{}", renderer);
  _logger->info("opengl version:\t{}", version);
  _logger->info("glsl version:\t{}", glsl_version);
  _logger->info("==============================================================");
}

} // namespace sbx
