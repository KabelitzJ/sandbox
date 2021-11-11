#include "rendering_module.hpp"

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <core/logger.hpp>
#include <core/events.hpp>

#include "render_system.hpp"

namespace sbx {

rendering_module::rendering_module() {
  
}

void rendering_module::initialize() {
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    logger::critical("Could not initialize glad");
    return;
  }

  _log_context_info();

  glClearColor(0.95f, 0.5f, 0.43f, 1.0f);

  add_listener<window_resized_event>([this](const auto& e) {
    glViewport(0, 0, e.width, e.height);
  });

  add_listener<clear_color_changed_event>([this](const auto& e) {
    glClearColor(e.color.r, e.color.g, e.color.b, e.color.a);
  });
  
  add_system<render_system>();
}

void rendering_module::terminate() {
  
}

void rendering_module::_log_context_info() const {
  const auto* vendor = glGetString(GL_VENDOR);
  const auto* renderer = glGetString(GL_RENDERER);
  const auto* version = glGetString(GL_VERSION);
  const auto* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

  logger::info("======================== context info ========================");
  logger::info("gpu vendor:\t{}", vendor);
  logger::info("gpu:\t\t{}", renderer);
  logger::info("opengl version:\t{}", version);
  logger::info("glsl version:\t{}", glsl_version);
  logger::info("==============================================================");
}

} // namespace sbx
