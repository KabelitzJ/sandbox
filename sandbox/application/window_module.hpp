#ifndef SBX_APPLICATION_WINDOW_MODULE_HPP_
#define SBX_APPLICATION_WINDOW_MODULE_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <core/module.hpp>

namespace sbx {

class window_module final : public module {

public:

  window_module();
  ~window_module();

  void initialize() override;
  void terminate() override;

private:

  GLFWwindow* _handle;

};

} // namespace sbx

#endif // SBX_APPLICATION_WINDOW_MODULE_HPP_
