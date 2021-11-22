#ifndef SBX_WINDOW_WINDOW_MODULE_HPP_
#define SBX_WINDOW_WINDOW_MODULE_HPP_

#include <GLFW/glfw3.h>

#include <core/module.hpp>
#include <core/logger.hpp>

namespace sbx {

class window_module final : public module {

public:

  window_module();
  ~window_module() = default;

  void initialize() override;
  void terminate() override;

private:

  GLFWwindow* _handle;

};

} // namespace sbx

#endif // SBX_WINDOW_WINDOW_MODULE_HPP_
