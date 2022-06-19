#ifndef DEMO_CONTEXT_HPP_
#define DEMO_CONTEXT_HPP_

#include <GLFW/glfw3.h>

namespace demo {

/**
 * @brief RAII wrapper for GLFW context.
 */
class context {

public:

  /**
   * @brief Initializes GLFW context and checks for vulkan support.
   */
  context() {
    _initialize();
  }

  /**
   * @brief Terminates GLFW context.
   */
  ~context() {
    _terminate();
  }

private:

  void _initialize() {
    if (!glfwInit()) {
      throw std::runtime_error{"Failed to initialize GLFW"};
    }

    if (!glfwVulkanSupported()) {
      throw std::runtime_error{"Vulkan is not supported"};
    }
  }

  void _terminate() {
    glfwTerminate();
  }

}; // class context

} // namespace demo

#endif // DEMO_CONTEXT_HPP_
