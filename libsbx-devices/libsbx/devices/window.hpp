#ifndef LIBSBX_DEVICES_WINDOW_HPP_
#define LIBSBX_DEVICES_WINDOW_HPP_

#include <string>
#include <vector>
#include <stdexcept>
#include <cinttypes>

#include <GLFW/glfw3.h>

namespace sbx::devices {

struct window_create_info {
  std::string title{};
  std::uint32_t width{};
  std::uint32_t height{};
};

class window {

public:

  window(const window_create_info& create_info) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(create_info.width, create_info.height, create_info.title.c_str(), nullptr, nullptr);

    if (!_window) {
      throw std::runtime_error("Failed to create window");
    }

    _setup_callbacks();
  }

  ~window() {
    glfwDestroyWindow(_window);
  }

  bool should_close() const {
    return glfwWindowShouldClose(_window);
  }

  void set_title(const std::string& title) {
    glfwSetWindowTitle(_window, title.c_str());
  }

  std::vector<const char*> get_requirements() const {
    auto requirements = std::vector<const char*>{};

    return requirements;
  }

private:

  void _setup_callbacks() {
    
  }

  GLFWwindow* _window{};

}; // class window

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_WINDOW_HPP_
